#include "BreedingWidget.h"
#include "BreedingSlotWidget.h"
#include "PalSlotWidget.h"
#include "Components/VerticalBox.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Pal.h"
#include "BreedingFarm.h"

void UBreedingWidget::NativeConstruct()
{
	Super::NativeConstruct();
	bIsFocusable = true;

	// 1. 교배 버튼 이벤트 연결
	if (Btn_Breed)
	{
		Btn_Breed->OnClicked.RemoveAll(this);
		Btn_Breed->OnClicked.AddDynamic(this, &UBreedingWidget::OnBreedButtonClicked);
	}

	// 2. 플레이어 인벤토리 찾기 및 연결
	if (APawn* PlayerPawn = GetOwningPlayerPawn())
	{
		PlayerPalStorageComp = PlayerPawn->FindComponentByClass<UPalStorageComponent>();
		if (PlayerPalStorageComp.IsValid())
		{
			PlayerPalStorageComp->OnPalStorageUpdated.RemoveAll(this);
			PlayerPalStorageComp->OnPalStorageUpdated.AddDynamic(this, &UBreedingWidget::RefreshPalList);
		}
	}

	// 3. 내 인벤토리(플레이어 팰 리스트) 즉시 갱신
	RefreshPalList();

	
}
void UBreedingWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// 필수 위젯 유효성 체크
	if (!TargetFarm || !PB_EggProgress || !Text_BreedBtn || !Text_Status) return;

	// --- 1. 알 프로그레스 바 로직 (역방향: 1.0 -> 0.0) ---
	if (TargetFarm->bIsBreeding)
	{
		float Remaining = TargetFarm->GetRemainingTime();
		float TotalTime = 180.0f; // 3분
		float Progress = Remaining / TotalTime;

		PB_EggProgress->SetPercent(Progress);
		PB_EggProgress->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		// 평상시에는 가득 찬 상태로 유지
		PB_EggProgress->SetPercent(1.0f);
		PB_EggProgress->SetVisibility(ESlateVisibility::Visible);
	}

	// --- 2. 버튼 및 상태 텍스트 로직 (통합) ---
	if (TargetFarm->bIsEggReady) // 알 준비됨
	{
		Text_BreedBtn->SetText(FText::FromString(TEXT("Take")));
		Text_Status->SetText(FText::FromString(TEXT("Egg is Ready!")));
		Btn_Breed->SetIsEnabled(true);
	}
	else if (TargetFarm->bIsBreeding) // 번식 중
	{
		Text_BreedBtn->SetText(FText::FromString(TEXT("Breeding...")));
		float Remaining = TargetFarm->GetRemainingTime();
		Text_Status->SetText(FText::Format(FText::FromString(TEXT("Next Egg: {0}s")), FMath::CeilToInt(Remaining)));
		Btn_Breed->SetIsEnabled(false);
	}
	else // 시작 전 대기 (라인 103 근처)
	{
		Text_BreedBtn->SetText(FText::FromString(TEXT("Start")));

		// 위젯이 아니라, TargetFarm이 들고 있는 '진짜 액터'를 체크하세요.
		if (TargetFarm->SpawnedParentA && TargetFarm->SpawnedParentB)
		{
			if (TargetFarm->SpawnedParentA->PalData.PalName == TargetFarm->SpawnedParentB->PalData.PalName)
			{
				Text_Status->SetText(FText::FromString(TEXT("Ready to Breed")));
				Btn_Breed->SetIsEnabled(true);
			}
			else
			{
				Text_Status->SetText(FText::FromString(TEXT("Species Mismatch!")));
				Btn_Breed->SetIsEnabled(false);
			}
		}
		else
		{
			Text_Status->SetText(FText::FromString(TEXT("Waiting for Parents...")));
			Btn_Breed->SetIsEnabled(false);
		}
	}
}

// 마우스 클릭이 아예 안 먹는지 확인하기 위한 로그
FReply UBreedingWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	UE_LOG(LogTemp, Error, TEXT("[BreedDebug] !!! MOUSE CLICK DETECTED !!! Key: %s"), *InMouseEvent.GetEffectingButton().ToString());
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

// 키보드(E키 등) 입력이 무시되는지 확인하기 위한 로그
FReply UBreedingWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	UE_LOG(LogTemp, Error, TEXT("[BreedDebug] !!! KEY DOWN DETECTED !!! Key: %s"), *InKeyEvent.GetKey().ToString());

	// 'E' 키를 눌렀을 때 상호작용 로직이 있다면 여기에 추가
	if (InKeyEvent.GetKey() == EKeys::E)
	{
		UE_LOG(LogTemp, Display, TEXT("[BreedDebug] E Key Pressed - Handling Interaction"));
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UBreedingWidget::OnBreedButtonClicked()
{
	if (!TargetFarm) return;

	if (TargetFarm->bIsEggReady)
	{
		TargetFarm->ClaimEgg(GetOwningPlayerPawn());
	}
	else
	{
		// [핵심] 이제 위젯은 부모 액터를 넘겨줄 필요가 없습니다! 
		// "농장아, 니 스토리지 확인하고 알아서 시작해!" 라고 딱 한 줄만 명령합니다.
		TargetFarm->StartBreeding();
	}
}

void UBreedingWidget::OnFarmStorageUpdated()
{
	// 필수 포인터 유효성 검사
	if (!TargetFarm || !TargetFarm->FarmStorageComponent || !SB_FarmParents || !PalSlotClass)
	{
		UE_LOG(LogTemp, Error, TEXT("[BreedingUI] Nurak."));
		return;
	}

	// 1. 기존에 생성된 슬롯들을 싹 비웁니다.
	SB_FarmParents->ClearChildren();

	// 2. 농장의 저장 용량(2칸)만큼 슬롯을 동적 생성합니다.
	int32 TotalCapacity = TargetFarm->FarmStorageComponent->StorageCapacity;

	for (int32 i = 0; i < TotalCapacity; i++)
	{
		UUserWidget* NewSlot = CreateWidget<UUserWidget>(this, PalSlotClass);

		if (UPalSlotWidget* SlotWidget = Cast<UPalSlotWidget>(NewSlot))
		{
			// 스포너 로직과 완벽히 동일하게 세팅
			SlotWidget->SlotIndex = i;
			SlotWidget->OwnerStorageComponent = TargetFarm->FarmStorageComponent;

			// 데이터 세팅
			FPalData DataToSend;
			if (TargetFarm->FarmStorageComponent->StoredPalsData.IsValidIndex(i))
			{
				DataToSend = TargetFarm->FarmStorageComponent->StoredPalsData[i];
			}

			SlotWidget->SetPalSlotData(DataToSend);
		}

		// 3. 스크롤 박스에 추가
		SB_FarmParents->AddChild(NewSlot);
	}
}

void UBreedingWidget::SetTargetFarm(ABreedingFarm* InFarm)
{
	if (!InFarm || !InFarm->FarmStorageComponent) return;

	TargetFarm = InFarm;

	// 기존 바인딩이 있다면 제거하고 확실하게 새로 바인딩
	TargetFarm->FarmStorageComponent->OnPalStorageUpdated.RemoveAll(this);
	TargetFarm->FarmStorageComponent->OnPalStorageUpdated.AddDynamic(this, &UBreedingWidget::OnFarmStorageUpdated);

	// ❌ 기존 수동 설정(Slot_Parent_A -> OwnerStorage = ...) 부분 삭제

	// 즉시 한 번 갱신해서 현재 데이터 보여주기
	OnFarmStorageUpdated();
}
void UBreedingWidget::RefreshPalList()
{
	if (!SB_PlayerPals || !PlayerPalStorageComp.IsValid() || !PalSlotClass) return;

	SB_PlayerPals->ClearChildren();
	int32 TotalCapacity = PlayerPalStorageComp->StorageCapacity;

	for (int32 i = 0; i < TotalCapacity; i++)
	{
		UUserWidget* NewSlot = CreateWidget<UUserWidget>(this, PalSlotClass);

		if (UPalSlotWidget* SlotWidget = Cast<UPalSlotWidget>(NewSlot))
		{
			SlotWidget->SlotIndex = i;
			SlotWidget->OwnerStorageComponent = PlayerPalStorageComp.Get();

			FPalData DataToSend;
			if (PlayerPalStorageComp->StoredPalsData.IsValidIndex(i))
			{
				DataToSend = PlayerPalStorageComp->StoredPalsData[i];
			}
			SlotWidget->SetPalSlotData(DataToSend);
		}
		SB_PlayerPals->AddChild(NewSlot);
	}
	UE_LOG(LogTemp, Log, TEXT("[BreedDebug] Pal List Refreshed: %d slots created"), TotalCapacity);
}