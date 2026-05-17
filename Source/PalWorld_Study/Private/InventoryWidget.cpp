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
	bIsFocusable = true;
}

void UInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_SaveGame)
	{
		Btn_SaveGame->OnClicked.AddDynamic(this, &UInventoryWidget::OnSaveButtonClicked);
	}

	APawn* PlayerPawn = GetOwningPlayerPawn();
	if (PlayerPawn)
	{
		PlayerInventoryComp = PlayerPawn->FindComponentByClass<UInventoryComponent>();
		if (PlayerInventoryComp.IsValid())
		{
			PlayerInventoryComp->OnInventoryUpdated.RemoveAll(this);
			PlayerInventoryComp->OnInventoryUpdated.AddDynamic(this, &UInventoryWidget::RefreshPlayerInventory);
		}

		PlayerPalStorageComp = PlayerPawn->FindComponentByClass<UPalStorageComponent>();
		if (PlayerPalStorageComp.IsValid())
		{
			PlayerPalStorageComp->OnPalStorageUpdated.RemoveAll(this);
			PlayerPalStorageComp->OnPalStorageUpdated.AddDynamic(this, &UInventoryWidget::RefreshPlayerPals);
		}
	}

	RefreshAllViews();
}

void UInventoryWidget::RefreshAllViews()
{
	RefreshPlayerInventory();
	RefreshPlayerPals();
}

void UInventoryWidget::RefreshPlayerInventory()
{
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

void UInventoryWidget::RefreshPlayerPals()
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
}

void UInventoryWidget::OnSaveButtonClicked()
{
	if (UPalStudyGameInstance* GI = Cast<UPalStudyGameInstance>(GetGameInstance()))
	{
		GI->SavePlayerData();
	}
}