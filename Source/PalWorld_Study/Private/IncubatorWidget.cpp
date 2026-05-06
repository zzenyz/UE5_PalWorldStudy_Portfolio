#include "IncubatorWidget.h"
#include "EggIncubator.h"
#include "ItemSlotWidget.h"
#include "Components/UniformGridPanel.h"
#include "Components/VerticalBox.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"

void UIncubatorWidget::NativeConstruct()
{
	Super::NativeConstruct();
	bIsFocusable = true;

	// 1. 버튼 이벤트 연결 (Breeding과 동일)
	if (Btn_Action)
	{
		Btn_Action->OnClicked.RemoveAll(this);
		Btn_Action->OnClicked.AddDynamic(this, &UIncubatorWidget::OnActionClicked);
	}

	// 2. 플레이어 인벤토리 찾기 및 델리게이트 연결 (Breeding과 동일)
	if (APawn* PlayerPawn = GetOwningPlayerPawn())
	{
		PlayerInventoryComp = PlayerPawn->FindComponentByClass<UInventoryComponent>();
		if (PlayerInventoryComp.IsValid())
		{
			PlayerInventoryComp->OnInventoryUpdated.RemoveAll(this);
			PlayerInventoryComp->OnInventoryUpdated.AddDynamic(this, &UIncubatorWidget::RefreshPlayerInventory);
		}
	}

	// 3. 내 인벤토리 즉시 갱신
	RefreshPlayerInventory();
}

void UIncubatorWidget::SetTargetIncubator(AEggIncubator* InIncubator)
{
	if (!InIncubator) return;
	TargetIncubator = InIncubator;

	if (TargetIncubator->IncubatorInventory)
	{
		TargetIncubator->IncubatorInventory->OnInventoryUpdated.RemoveAll(this);
		TargetIncubator->IncubatorInventory->OnInventoryUpdated.AddDynamic(this, &UIncubatorWidget::RefreshIncubatorSlot);
	}

	RefreshIncubatorSlot();
}

void UIncubatorWidget::RefreshPlayerInventory()
{
	// SpawnerInventoryWidget의 RefreshBaseInventory와 완벽히 동일한 동적 할당
	if (!Grid_PlayerItems || !PlayerInventoryComp.IsValid() || !ItemSlotClass) return;

	Grid_PlayerItems->ClearChildren();
	int32 TotalCapacity = PlayerInventoryComp->GetCapacity();

	for (int32 i = 0; i < TotalCapacity; i++)
	{
		UUserWidget* NewSlot = CreateWidget<UUserWidget>(this, ItemSlotClass);

		if (UItemSlotWidget* SlotWidget = Cast<UItemSlotWidget>(NewSlot))
		{
			SlotWidget->SlotIndex = i;
			SlotWidget->OwnerInvenComponent = PlayerInventoryComp.Get();

			FItemSlot DataToSend;
			if (PlayerInventoryComp->Slots.IsValidIndex(i))
			{
				DataToSend = PlayerInventoryComp->Slots[i];
			}
			SlotWidget->SetItemSlotData(DataToSend);
		}

		int32 Row = i / ColumnsPerRow;
		int32 Column = i % ColumnsPerRow;
		Grid_PlayerItems->AddChildToUniformGrid(NewSlot, Row, Column);
	}
}

void UIncubatorWidget::RefreshIncubatorSlot()
{
	// 1. 필수 포인터들이 모두 있는지 검사 (인큐베이터 인벤토리 포함)
	if (!TargetIncubator || !Box_IncubatorSlot || !ItemSlotClass || !TargetIncubator->IncubatorInventory) return;

	// 2. 기존 박스 비우기
	Box_IncubatorSlot->ClearChildren();

	// 3. 새 슬롯 1개 동적 생성
	UUserWidget* NewSlot = CreateWidget<UUserWidget>(this, ItemSlotClass);
	if (UItemSlotWidget* SlotWidget = Cast<UItemSlotWidget>(NewSlot))
	{
		// 🔥 [핵심] 방금 만든 진짜 인벤토리와 0번 칸(첫 번째 칸)을 연결해 줍니다!
		SlotWidget->OwnerInvenComponent = TargetIncubator->IncubatorInventory;
		SlotWidget->SlotIndex = 0;

		FItemSlot DataToSend;
		if (TargetIncubator->IncubatorInventory->Slots.IsValidIndex(0))
		{
			DataToSend = TargetIncubator->IncubatorInventory->Slots[0];
		}
		SlotWidget->SetItemSlotData(DataToSend);
	}

	// 4. 화면(Vertical Box)에 추가
	Box_IncubatorSlot->AddChild(NewSlot);
}

void UIncubatorWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// BreedingWidget의 NativeTick 로직과 동일
	if (!TargetIncubator || !PB_HatchingProgress || !Text_BtnAction || !Text_Timer) return;

	if (TargetIncubator->bHasEgg)
	{
		float Remaining = TargetIncubator->GetWorldTimerManager().GetTimerRemaining(TargetIncubator->HatchTimerHandle);
		float TotalTime = TargetIncubator->HatchingTime;
		float Progress = 1.0f - (Remaining / TotalTime);

		PB_HatchingProgress->SetPercent(Progress);
		Text_Timer->SetText(FText::Format(FText::FromString(TEXT("{0}s")), FMath::CeilToInt(Remaining)));

		if (TargetIncubator->bIsHatched)
		{
			Text_BtnAction->SetText(FText::FromString(TEXT("Take")));
			Btn_Action->SetIsEnabled(true);
		}
		else
		{
			Text_BtnAction->SetText(FText::FromString(TEXT("Hatching...")));
			Btn_Action->SetIsEnabled(false);
		}
	}
	else
	{
		PB_HatchingProgress->SetPercent(0.0f);
		Text_Timer->SetText(FText::FromString(TEXT("0s")));
		Text_BtnAction->SetText(FText::FromString(TEXT("Drop Egg Here")));
		Btn_Action->SetIsEnabled(false);
	}
}

void UIncubatorWidget::OnActionClicked()
{
	if (!TargetIncubator) return;

	// 부화가 완료되었을 때만 작동
	if (TargetIncubator->bIsHatched)
	{
		TargetIncubator->ClaimPal(GetOwningPlayerPawn());

		RefreshIncubatorSlot();

		UE_LOG(LogTemp, Warning, TEXT("[Incubator] Pal Claimed! UI remains open as requested."));
	}
}