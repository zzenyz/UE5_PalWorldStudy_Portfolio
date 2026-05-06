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
	// 1. 연결 로직 (이전과 동일)
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

	// 2. 화면 갱신 (Hidden -> Visible 후 이걸 호출하면 해결됨!)
	RefreshAllViews();
}

void USpawnerInventoryWidget::RefreshAllViews()
{
	Super::RefreshAllViews(); // 부모(플레이어) 먼저
	RefreshBaseInventory();   // 스포너 인벤
	RefreshBasePals();        // 스포너 팰
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

			// 🔥 [추가됨] 주인 컴포넌트 등록! (드래그 앤 드롭 필수)
			SlotWidget->OwnerInvenComponent = BaseInventoryComp.Get();

			// 📦 [원래 있던 코드 복구] 데이터를 찾아서 슬롯에 넣어줘야 함!
			FItemSlot DataToSend;
			if (BaseInventoryComp->Slots.IsValidIndex(i))
			{
				DataToSend = BaseInventoryComp->Slots[i];
			}
			SlotWidget->SetItemSlotData(DataToSend); // 👈 이게 꼭 있어야 함!
		}

		int32 Row = i / ColumnsPerRow;
		int32 Column = i % ColumnsPerRow;
		Grid_BaseItems->AddChildToUniformGrid(NewSlot, Row, Column);
	}
}

void USpawnerInventoryWidget::RefreshBasePals()
{
	// 4-1. 보관함 (Stored)
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

				// 🔥 [추가됨] 보관함 주인 등록
				SlotWidget->OwnerStorageComponent = BaseStorageComp.Get();

				// 📦 [원래 있던 코드 복구]
				FPalData DataToSend;
				if (BaseStorageComp->StoredPalsData.IsValidIndex(i))
				{
					DataToSend = BaseStorageComp->StoredPalsData[i];
				}
				SlotWidget->SetPalSlotData(DataToSend); // 👈 필수!
			}
			SB_BaseStoredPals->AddChild(NewSlot);
		}
	}

	// 4-2. 작업장 (Active)
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

				// 🔥 [추가됨] 작업장 주인 등록
				SlotWidget->OwnerStorageComponent = BaseActiveComp.Get();

				// 📦 [원래 있던 코드 복구]
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