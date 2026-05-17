#include "SpawnerInventoryWidget.h"
#include "Components/UniformGridSlot.h"
#include "ItemslotWidget.h"
#include "PalSlotWidget.h"

USpawnerInventoryWidget::USpawnerInventoryWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bIsFocusable = true;
}

void USpawnerInventoryWidget::SetupSpawnerInfo(UInventoryComponent* InInventory, UPalStorageComponent* InStorage, UPalStorageComponent* InActiveStorage)
{
	if (InInventory)
	{
		BaseInventoryComp = InInventory;
		BaseInventoryComp->OnInventoryUpdated.RemoveAll(this);
		BaseInventoryComp->OnInventoryUpdated.AddDynamic(this, &USpawnerInventoryWidget::RefreshBaseInventory);
	}

	if (InStorage)
	{
		BaseStorageComp = InStorage;
		BaseStorageComp->OnPalStorageUpdated.RemoveAll(this);
		BaseStorageComp->OnPalStorageUpdated.AddDynamic(this, &USpawnerInventoryWidget::RefreshBasePals);
	}

	if (InActiveStorage)
	{
		BaseActiveComp = InActiveStorage;
		BaseActiveComp->OnPalStorageUpdated.RemoveAll(this);
		BaseActiveComp->OnPalStorageUpdated.AddDynamic(this, &USpawnerInventoryWidget::RefreshBasePals);
	}

	RefreshAllViews();
}

void USpawnerInventoryWidget::RefreshAllViews()
{
	Super::RefreshAllViews();
	RefreshBaseInventory();
	RefreshBasePals();
}

void USpawnerInventoryWidget::RefreshBaseInventory()
{
	if (!Grid_BaseItems || !BaseInventoryComp.IsValid() || !ItemSlotClass) return;

	Grid_BaseItems->ClearChildren();

	int32 TotalCapacity = BaseInventoryComp->GetCapacity();

	for (int32 i = 0; i < TotalCapacity; i++)
	{
		UUserWidget* NewSlot = CreateWidget<UUserWidget>(this, ItemSlotClass);

		if (UItemSlotWidget* SlotWidget = Cast<UItemSlotWidget>(NewSlot))
		{
			SlotWidget->SlotIndex = i;
			SlotWidget->OwnerInvenComponent = BaseInventoryComp.Get();

			FItemSlot DataToSend;
			if (BaseInventoryComp->Slots.IsValidIndex(i))
			{
				DataToSend = BaseInventoryComp->Slots[i];
			}
			SlotWidget->SetItemSlotData(DataToSend);
		}

		int32 Row = i / ColumnsPerRow;
		int32 Column = i % ColumnsPerRow;
		Grid_BaseItems->AddChildToUniformGrid(NewSlot, Row, Column);
	}
}

void USpawnerInventoryWidget::RefreshBasePals()
{
	// 보관함 (Stored)
	if (SB_BaseStoredPals && BaseStorageComp.IsValid() && PalSlotClass)
	{
		SB_BaseStoredPals->ClearChildren();
		int32 Cap = BaseStorageComp->StorageCapacity;

		for (int32 i = 0; i < Cap; i++)
		{
			UUserWidget* NewSlot = CreateWidget<UUserWidget>(this, PalSlotClass);

			if (UPalSlotWidget* SlotWidget = Cast<UPalSlotWidget>(NewSlot))
			{
				SlotWidget->SlotIndex = i;
				SlotWidget->OwnerStorageComponent = BaseStorageComp.Get();

				FPalData DataToSend;
				if (BaseStorageComp->StoredPalsData.IsValidIndex(i))
				{
					DataToSend = BaseStorageComp->StoredPalsData[i];
				}
				SlotWidget->SetPalSlotData(DataToSend);
			}
			SB_BaseStoredPals->AddChild(NewSlot);
		}
	}

	// 작업장 (Active)
	if (SB_BaseSpawnedPals && BaseActiveComp.IsValid() && PalSlotClass)
	{
		SB_BaseSpawnedPals->ClearChildren();
		int32 Cap = BaseActiveComp->StorageCapacity;

		for (int32 i = 0; i < Cap; i++)
		{
			UUserWidget* NewSlot = CreateWidget<UUserWidget>(this, PalSlotClass);

			if (UPalSlotWidget* SlotWidget = Cast<UPalSlotWidget>(NewSlot))
			{
				SlotWidget->SlotIndex = i;
				SlotWidget->OwnerStorageComponent = BaseActiveComp.Get();

				FPalData DataToSend;
				if (BaseActiveComp->StoredPalsData.IsValidIndex(i))
				{
					DataToSend = BaseActiveComp->StoredPalsData[i];
				}
				SlotWidget->SetPalSlotData(DataToSend);
			}
			SB_BaseSpawnedPals->AddChild(NewSlot);
		}
	}
}