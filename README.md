# 🎮 UE5_PalWorldStudy_Portfolio
> **C++ 기반 언리얼 엔진 5를 활용한 팰월드(Palworld) 핵심 시스템 모작 프로젝트**

<div align="center">
  <img src="https://img.shields.io/badge/Unreal%20Engine%205.3-0E1128?style=for-the-badge&logo=UnrealEngine&logoColor=white"/>
  <img src="https://img.shields.io/badge/C++-00599C?style=for-the-badge&logo=cplusplus&logoColor=white"/>
  <img src="https://img.shields.io/badge/Git-F05032?style=for-the-badge&logo=git&logoColor=white"/>
</div>

## 목차
1️⃣ [프로젝트 개요](#1️⃣-프로젝트-개요-overview) <br>
2️⃣ [시연 영상 및 스크린샷](#2️⃣-시연-영상-및-스크린샷-demo--screenshots) <br>
3️⃣ [주요 기능 및 시스템 흐름도](#3️⃣-주요-기능-및-시스템-흐름도-features--flowchart) <br>
4️⃣ [사용 기술 및 개발 환경](#4️⃣-사용-기술-및-개발-환경-tech-stack) <br>
5️⃣ [핵심 트러블슈팅](#5️⃣-핵심-트러블슈팅-troubleshooting) <br>
6️⃣ [상세 문서 (노션)](#6️⃣-상세-문서-및-링크-notion--docs) <br>
7️⃣ [담당 역할](#7️⃣-담당-역할-roles)

---

## 1️⃣ 프로젝트 개요 (Overview)

| 항목 | 상세 내용 |
| :--- | :--- |
| **개발 기간** | 2025.09.22 ~ 2025.10.02 (약 2주) |
| **개발 인원** | 5명 (팀 프로젝트) |
| **프로젝트 소개** | 원작 '팰월드'의 핵심 재미 요소인 **포획, 전투, 번식 메커니즘**을 분석하고, 이를 언리얼 엔진 5와 C++을 활용해 객체 지향적 구조로 모작한 프로젝트입니다. |

---

## 2️⃣ 시연 영상 및 스크린샷 (Demo & Screenshots)

### 🎬 게임 플레이 시연
[![PalStudy 시연 영상](https://img.youtube.com/vi/K0criVOJP2o/0.jpg)](https://youtu.be/K0criVOJP2o)

## 3️⃣ 주요 기능 및 시스템 흐름도 (Features & Flowchart)

### 📌 시스템 흐름도
<fieldset>
  <img width="2090" alt="Core System Flowchart 1" src="https://github.com/user-attachments/assets/31645aff-a7ef-4260-b1d1-df2223be329c" />
  <img width="2091" alt="Core System Flowchart 2" src="https://github.com/user-attachments/assets/594fb83b-bf3a-4497-b488-27b6e3561c55" />
</fieldset>

### 💡 주요 구현 기능
* **비동기적 팰 포획 파이프라인 및 상태 관리**
  * 스피어 명중 시 대상 팰의 상태(`bIsBeingCaptured`)를 선제적으로 검증 및 락(Lock)을 수행하여, 멀티 연산 환경에서 발생할 수 있는 예기치 않은 중복 포획 버그 및 레이스 컨디션을 방지했습니다.
* **액터 생명주기(Lifecycle) 관리 및 상황별 동적 소환(Spawn)**
  * 월드에 존재하는 포획된 팰을 무거운 3D Actor 상태로 계속 유지하는 대신, 메모리 최적화를 위해 무대 뒤에서는 가벼운 구조체 데이터(`FPalData`)로 변환하여 보관함에 저정하도록 설계했습니다. 필요 시에만 동적으로 스폰하여 메모리 효율성을 극대화했습니다.
* **상태(State) 기반 데이터 주입 및 행동 트리 동적 제어**
  * 팰의 상태(야생/포획) 및 현재 목적(전투/노동)에 따라 유기적으로 Blackboard 데이터를 변경하고, AI Behavior Tree를 동적으로 제어하는 유연한 AI 시스템을 구축했습니다.

---

## 4️⃣ 사용 기술 및 개발 환경 (Tech Stack)

* **Engine:** Unreal Engine 5.3
* **Language:** C++
* **Version Control:** Git, GitHub
* **Communication:** Notion, Discord

---

## 5️⃣ 핵심 트러블슈팅 (Troubleshooting)

### 이벤트 기반(Delegate) UI 동기화 및 결합도 완화

> **요약:** 블루프린트 위젯 간의 무분별한 틱(Tick) 기반 폴링 구조를 언리얼 델리게이트 기반의 옵저버 패턴으로 리팩토링하여 UI 의존성 분리 및 CPU 연산 최적화를 달성했습니다.

* **문제 상황 (Problem)**
  * 초기 단계에서는 블루프린트 위젯 간의 복잡한 상속 구조로 인해 강한 결합(Tight Coupling)이 발생했습니다. 특히 UI가 데이터를 갱신하기 위해 매 프레임(`Tick`) 플레이어 객체를 참조하고 캐스팅해야 하는 심각한 연산 비효율이 존재했습니다.
* **원인 분석 (Root Cause)**
  * UI가 데이터의 변경 시점을 알지 못해 매 프레임 데이터를 직접 확인하는 **폴링(Polling) 방식**을 채택하면서 발생한 문제였습니다. 이 과정에서 UI 위젯이 플레이어 캐릭터와 컴포넌트를 직접 참조하게 되어 **하드 레퍼런스(Hard Reference)**로 인한 강한 결합이 발생했습니다. 특히, 매 틱마다 무거운 **캐스팅(Cast) 연산**이 누적되면서 불필요한 CPU 성능 낭비와 프레임 드랍의 원인이 되었습니다.
* **해결 방법 (Solution)**
  * 데이터의 원본을 관리하는 컴포넌트(`UInventoryComponent`, `UPalStorageComponent`)에 델리게이트 이벤트를 선언했습니다. UI 위젯은 초기화(`NativeConstruct`) 시점에 이 이벤트를 구독(`AddDynamic`)만 해두고, 데이터가 실제로 변동(Setter 호출 등)될 때만 `Broadcast()`가 호출되어 UI가 스스로 갱신되도록 **옵저버 패턴(Observer Pattern)**을 도입했습니다.
* **결과 (Result)**
  * UI는 데이터가 '어떻게' 변경되었는지 내부 로직을 알 필요 없이, 알림이 올 때만 화면을 새로고침하므로 **의존성을 완벽히 분리(Decoupling)**했습니다. 결과적으로 불필요한 프레임별 캐스팅 오버헤드를 제거하여 CPU 점유율 최적화 및 유지보수성을 극대화했습니다.

<details>
<summary>🛠️ 핵심 해결 코드 보기 (클릭)</summary>

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
```

</details>

---

## 6️⃣ 상세 문서 및 링크 (Notion & Docs)

📄 **[프로젝트 상세 노션 페이지 바로가기](https://www.notion.so/CHOI-GAYEON-3480e502a9f680feacb5dfebf7fa8eb9)**

---

## 7️⃣ 담당 역할 (Roles)

* **[최가연] (클라이언트 프로그래머 - 핵심 콘텐츠 구현)**
  * **AI 및 행동 트리(Behavior Tree) 기반 팰 자율 행동 로직 구현**
    * 상태(야생/포획) 및 목적(전투/노동)에 따라 유기적으로 변화하는 AI 시스템 구축
    * EQS(Environment Query System) 등을 활용한 최적의 이동 및 타겟팅 로직 설계
  * **팰 스켈레탈 메쉬 및 애니메이션 파이프라인 구축**
    * C++ 클래스와 애니메이션 블루프린트(`AnimInstance`) 연동을 통한 동적 상태별 애니메이션 제어
  * **비동기적 팰 포획 시스템 및 예외 처리 구현**
    * 스피어 명중 시 상태 검증 코드를 통해 중복 포획 버그 및 레이스 컨디션 원천 차단
  * **팰 노동/작업 배치(Work Assignment) 시스템 구현**
    * 월드 내 다양한 생산 오브젝트와 팰의 상호작용 및 동적 거점 배치 로직 개발
  * **구조체 기반 팰 번식(Breeding) 메커니즘 설계**
    * 부모 팰의 데이터(개체값, 스킬 등)를 연산하여 자식 팰에게 유전시키는 상속 알고리즘 구현
