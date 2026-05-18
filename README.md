# UE5_PalWorldStudy_Portfolio
> [프로젝트를 설명하는 핵심 한 줄 소개, 예: C++ 기반 언리얼 엔진 실시간 멀티플레이 게임]

## 목차
1️⃣ [프로젝트 개요](#1️⃣-프로젝트-개요-overview)
2️⃣ [시연 영상 및 스크린샷](#2️⃣-시연-영상-및-스크린샷-demo--screenshots)
3️⃣ [주요 기능 및 시스템 흐름도](#3️⃣-주요-기능-및-시스템-흐름도-features--flowchart)
4️⃣ [사용 기술 및 개발 환경](#4️⃣-사용-기술-및-개발-환경-tech-stack)
5️⃣ [핵심 트러블슈팅](#5️⃣-핵심-트러블슈팅-troubleshooting)
6️⃣ [상세 문서 (노션)](#6️⃣-상세-문서-및-링크-notion--docs)
7️⃣ [담당 역할](#7️⃣-담당-역할-roles)

---

## 1️⃣ 프로젝트 개요 (Overview)
* **개발 기간:** [2025.09.22 ~ 2025.10.02 (약 2주)]
* **개발 인원:** [ 5명 ]
* **프로젝트 소개:** 
  * [- **팰월드 시스템 모작**

원작의 핵심인 포획, 전투, 번식 메커니즘을 언리얼 엔진으로 구현]

## 2️⃣ 시연 영상 및 스크린샷 (Demo & Screenshots)
### [게임 플레이 영상]
* [유튜브 링크 또는 전체 플레이 영상 GIF 삽입]



## 3️⃣ 주요 기능 및 시스템 흐름도 (Features & Flowchart)
### 시스템 흐름도
* <img width="2090" height="1176" alt="1" src="https://github.com/user-attachments/assets/31645aff-a7ef-4260-b1d1-df2223be329c" />

* <img width="2091" height="1169" alt="2" src="https://github.com/user-attachments/assets/594fb83b-bf3a-4497-b488-27b6e3561c55" />

### 주요 구현 기능
* **[기능 1, 비동기적 팰 포획 파이프라인 및 상태 관리]**: [스피어 명중 시 대상 팰의 상태(bIsBeingCaptured)를 선제적으로 검증하여 예기치 않은 중복 포획 버그를 방지했습니다.]
* **[기능 2, 액터 생명주기(Lifecycle) 관리 및 상황별 동적 소환(Spawn)]**: [포획한 팰을 무거운 3D 액터로 계속 유지하는 대신, 메모리 최적화를 위해 가벼운 구조체 데이터(FPalData)로 보관함에 저장하도록 시스템을 설계했습니다.]
* **[기능 3, 상태(STATE) 기반 데이터 주입 및 행동 트리 동적 제어]**: [팰의 상태(Wild/Captured) 및 목적(전투/노동)에 따라 유기적으로 변화하는 AI 시스템을 구축했습니다.]

## 4️⃣ 사용 기술 및 개발 환경 (Tech Stack)
* **Engine:** [Unreal Engine 5.3]
* **Language:** [C++]
* **Version Control:** [Git, GitHub]
* **Communication:** [Notion, Discord]

## 5️⃣ 핵심 트러블슈팅 (Troubleshooting)
### 1. [이벤트 기반(DELEGATE) UI 동기화 및 결합도 완화]
* **문제 상황:** [초기 단계에서는 블루프린트 위젯 간의 복잡한 상속 구조로 인해 강한 결합(Tight Coupling)이 발생했습니다. 특히 UI가 데이터를 갱신하기 위해 매 프레임(Tick) 플레이어 객체를 참조하고 캐스팅해야 하는 심각한 연산 비효율이 존재 했습니다.]
* **원인 분석:** [UI가 데이터의 변경 시점을 알지 못해 매 프레임 데이터를 직접 확인하는 폴링(Polling) 방식을 채택하면서 발생한 문제였습니다. 이 과정에서 UI 위젯이 플레이어 캐릭터와 컴포넌트를 직접 참조하게 되어 하드 레퍼런스로 인한 강한 결합이 발생했습니다. 특히, 매 틱(Tick)마다 무거운 캐스팅(Cast) 연산이 누적되면서 불필요한 CPU 성능 낭비와 프레임 드랍의 원인이 되었습니다.]
* **해결 방법:** [데이터의 원본을 쥐고 있는 컴포넌트(UInventoryComponent 등)에 OnInventoryUpdated라는 이벤트를 선언했습니다. UI 위젯은 초기화(NativeConstruct) 시점에 이 이벤트를 구독(AddDynamic)만 해둡니다. 데이터가 변동될 때만 Broadcast()가 호출되어 UI가 갱신됩니다. ]
* **결과:** [이제 UI는 '데이터가 어떻게 변했는지' 알 필요 없이 알림이 올 때만 화면을 새로고침하므로, 스파게티 코드를 원천 차단하는 최적화가 이루어졌습니다.]

<details>
<summary>핵심 해결 코드 보기 (클릭)</summary>
<div markdown="1">

```cpp
// UInventoryWidget::NativeConstruct 내부 로직
// [리팩토링] 데이터 갱신 시에만 UI가 반응하도록 델리게이트(옵저버 패턴) 구독

APawn* PlayerPawn = GetOwningPlayerPawn();
if (PlayerPawn)
{
    // 1. 일반 아이템 인벤토리 델리게이트 바인딩
    PlayerInventoryComp = PlayerPawn->FindComponentByClass<UInventoryComponent>();
    if (PlayerInventoryComp.IsValid())
    {
        // 중복 바인딩 방지를 위해 기존 연결 제거 후 새로 구독
        PlayerInventoryComp->OnInventoryUpdated.RemoveAll(this);
        PlayerInventoryComp->OnInventoryUpdated.AddDynamic(this, &UInventoryWidget::RefreshPlayerInventory);
    }

    // 2. 팰(Pal) 보관함 델리게이트 독립적 바인딩
    PlayerPalStorageComp = PlayerPawn->FindComponentByClass<UPalStorageComponent>();
    if (PlayerPalStorageComp.IsValid())
    {
        // 성격이 다른 데이터이므로 독립적인 이벤트로 갱신 분리
        PlayerPalStorageComp->OnPalStorageUpdated.RemoveAll(this);
        PlayerPalStorageComp->OnPalStorageUpdated.AddDynamic(this, &UInventoryWidget::RefreshPlayerPals);
    }
}
