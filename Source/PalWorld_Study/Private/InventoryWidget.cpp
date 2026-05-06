#include "InventoryWidget.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "ItemslotWidget.h"
#include "PalSlotWidget.h"
#include "PalStudySaveGame.h"
#include "PalStudyGameInstance.h"

UInventoryWidget::UInventoryWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// 위젯이 키보드 포커스를 받을 수 있도록 설정합니다.
	bIsFocusable = true;
}

void UInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 1. 버튼 클릭 이벤트 연결
	if (Btn_SaveGame)
	{
		Btn_SaveGame->OnClicked.AddDynamic(this, &UInventoryWidget::OnSaveButtonClicked);
	}

	// 1. 내 캐릭터(플레이어) 찾기
	APawn* PlayerPawn = GetOwningPlayerPawn();
	if (PlayerPawn)
	{
		// 2. 플레이어 인벤토리 컴포넌트 찾기 & 연결
		PlayerInventoryComp = PlayerPawn->FindComponentByClass<UInventoryComponent>();
		if (PlayerInventoryComp.IsValid())
		{
			PlayerInventoryComp->OnInventoryUpdated.RemoveAll(this);
			PlayerInventoryComp->OnInventoryUpdated.AddDynamic(this, &UInventoryWidget::RefreshPlayerInventory);
		}

		// 3. 플레이어 팰 스토리지 찾기 & 연결
		PlayerPalStorageComp = PlayerPawn->FindComponentByClass<UPalStorageComponent>();
		if (PlayerPalStorageComp.IsValid())
		{
			// PalStorageComponent 헤더에 선언된 델리게이트 이름이 OnInventoryUpdated가 맞는지 확인 필요
			PlayerPalStorageComp->OnPalStorageUpdated.RemoveAll(this);
			PlayerPalStorageComp->OnPalStorageUpdated.AddDynamic(this, &UInventoryWidget::RefreshPlayerPals);
		}
	}

	// 4. 최초 1회 갱신
	RefreshAllViews();
}

void UInventoryWidget::RefreshAllViews()
{
	// 부모는 자기 담당(플레이어 것)만 갱신하면 됨
	RefreshPlayerInventory();
	RefreshPlayerPals();
}

void UInventoryWidget::RefreshPlayerInventory()
{
	if (!Grid_PlayerItems || !PlayerInventoryComp.IsValid() || !ItemSlotClass) return;

	Grid_PlayerItems->ClearChildren();

	// 전체 용량만큼 반복 (빈 칸 포함)
	int32 TotalCapacity = PlayerInventoryComp->GetCapacity();

	for (int32 i = 0; i < TotalCapacity; i++)
	{
		UUserWidget* NewSlot = CreateWidget<UUserWidget>(this, ItemSlotClass);

		if (UItemSlotWidget* SlotWidget = Cast<UItemSlotWidget>(NewSlot))
		{
			SlotWidget->SlotIndex = i;

			// 🔥 [추가] 슬롯에게 "네 주인은 이 인벤토리 컴포넌트야"라고 알려줘야 함!
			// (SlotWidget 헤더에 OwnerInventoryComp 변수가 public으로 있어야 함)
			SlotWidget->OwnerInvenComponent = PlayerInventoryComp.Get();

			FItemSlot DataToSend;
			if (PlayerInventoryComp->Slots.IsValidIndex(i))
			{
				DataToSend = PlayerInventoryComp->Slots[i];
			}
			SlotWidget->SetItemSlotData(DataToSend);
		}

		// 바둑판 배치
		int32 Row = i / ColumnsPerRow;
		int32 Column = i % ColumnsPerRow;
		Grid_PlayerItems->AddChildToUniformGrid(NewSlot, Row, Column);
	}
}

void UInventoryWidget::RefreshPlayerPals()
{
	if (!SB_PlayerPals || !PlayerPalStorageComp.IsValid() || !PalSlotClass) return;

	SB_PlayerPals->ClearChildren();

	// 팰 보관함 용량 (5마리)
	int32 TotalCapacity = PlayerPalStorageComp->StorageCapacity;

	for (int32 i = 0; i < TotalCapacity; i++)
	{
		UUserWidget* NewSlot = CreateWidget<UUserWidget>(this, PalSlotClass);

		if (UPalSlotWidget* SlotWidget = Cast<UPalSlotWidget>(NewSlot))
		{
			SlotWidget->SlotIndex = i;

			// 🔥 [추가] 슬롯에게 "네 주인은 이 팰 스토리지야"라고 알려줘야 함!
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
}

// InventoryWidget.cpp
void UInventoryWidget::OnSaveButtonClicked()
{
	// ★ 직접 저장하지 말고, 우리가 정성껏 만든 GameInstance의 함수를 부르세요.
	if (UPalStudyGameInstance* GI = Cast<UPalStudyGameInstance>(GetGameInstance()))
	{
		GI->SavePlayerData();
		UE_LOG(LogTemp, Warning, TEXT("save call complete!"));
	}
}