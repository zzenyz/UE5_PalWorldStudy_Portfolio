#include "InventoryComponent.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	Slots.SetNum(Capacity);
}

bool UInventoryComponent::AddItem(UItemDataAsset* ItemAsset, int32 Quantity)
{
	if (!ItemAsset || Quantity <= 0) return false;

	int32 QuantityToAdd = Quantity;

	// 기존 슬롯 스태킹
	for (FItemSlot& Slot : Slots)
	{
		if (Slot.ItemAsset == ItemAsset && Slot.Quantity < ItemAsset->MaxStackSize)
		{
			int32 CanAddAmount = ItemAsset->MaxStackSize - Slot.Quantity;
			int32 AddAmount = FMath::Min(QuantityToAdd, CanAddAmount);
			Slot.Quantity += AddAmount;
			QuantityToAdd -= AddAmount;

			if (QuantityToAdd <= 0)
			{
				if (OnInventoryUpdated.IsBound()) OnInventoryUpdated.Broadcast();
				return true;
			}
		}
	}

	// 빈 슬롯에 추가
	for (FItemSlot& Slot : Slots)
	{
		if (Slot.ItemAsset == nullptr)
		{
			int32 AddAmount = FMath::Min(QuantityToAdd, ItemAsset->MaxStackSize);
			Slot.ItemAsset = ItemAsset;
			Slot.Quantity = AddAmount;
			QuantityToAdd -= AddAmount;

			if (QuantityToAdd <= 0)
			{
				if (OnInventoryUpdated.IsBound()) OnInventoryUpdated.Broadcast();
				return true;
			}
		}
	}

	// 인벤토리 꽉 참 (부분 추가 처리)
	if (QuantityToAdd < Quantity)
	{
		if (OnInventoryUpdated.IsBound()) OnInventoryUpdated.Broadcast();
	}

	return false;
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

bool UInventoryComponent::SwapItems(int32 SourceIndex, UInventoryComponent* DestinationInventory, int32 DestinationIndex)
{
	if (!DestinationInventory || !this->Slots.IsValidIndex(SourceIndex)) return false;

	FItemSlot& SourceSlot = this->Slots[SourceIndex];

	// 단일 인벤토리 내 교환
	if (this == DestinationInventory)
	{
		if (!this->Slots.IsValidIndex(DestinationIndex)) return false;

		FItemSlot& DestinationSlot = this->Slots[DestinationIndex];

		if (SourceSlot.ItemAsset != nullptr && DestinationSlot.ItemAsset != nullptr &&
			SourceSlot.ItemAsset == DestinationSlot.ItemAsset &&
			DestinationSlot.Quantity < DestinationSlot.ItemAsset->MaxStackSize)
		{
			int32 TransferableAmount = DestinationSlot.ItemAsset->MaxStackSize - DestinationSlot.Quantity;
			int32 AmountToMove = FMath::Min(SourceSlot.Quantity, TransferableAmount);

			if (AmountToMove > 0)
			{
				DestinationSlot.Quantity += AmountToMove;
				SourceSlot.Quantity -= AmountToMove;

				if (SourceSlot.Quantity <= 0)
				{
					SourceSlot.ItemAsset = nullptr;
					SourceSlot.Quantity = 0;
				}

				if (OnInventoryUpdated.IsBound()) OnInventoryUpdated.Broadcast();
				return true;
			}
		}

		this->Slots.Swap(SourceIndex, DestinationIndex);

		if (OnInventoryUpdated.IsBound()) OnInventoryUpdated.Broadcast();
		return true;
	}
	// 외부 인벤토리와 교환
	else
	{
		if (!DestinationInventory->Slots.IsValidIndex(DestinationIndex)) return false;

		FItemSlot& DestinationSlot = DestinationInventory->Slots[DestinationIndex];

		if (SourceSlot.ItemAsset != nullptr && DestinationSlot.ItemAsset != nullptr &&
			SourceSlot.ItemAsset == DestinationSlot.ItemAsset &&
			DestinationSlot.Quantity < DestinationSlot.ItemAsset->MaxStackSize)
		{
			int32 TransferableAmount = DestinationSlot.ItemAsset->MaxStackSize - DestinationSlot.Quantity;
			int32 AmountToMove = FMath::Min(SourceSlot.Quantity, TransferableAmount);

			if (AmountToMove > 0)
			{
				DestinationSlot.Quantity += AmountToMove;
				SourceSlot.Quantity -= AmountToMove;

				if (SourceSlot.Quantity <= 0)
				{
					SourceSlot.ItemAsset = nullptr;
					SourceSlot.Quantity = 0;
				}

				if (this->OnInventoryUpdated.IsBound())
					this->OnInventoryUpdated.Broadcast();
				if (DestinationInventory->OnInventoryUpdated.IsBound())
					DestinationInventory->OnInventoryUpdated.Broadcast();

				return true;
			}
		}

		Swap(SourceSlot, DestinationSlot);

		if (this->OnInventoryUpdated.IsBound())
			this->OnInventoryUpdated.Broadcast();
		if (DestinationInventory->OnInventoryUpdated.IsBound())
			DestinationInventory->OnInventoryUpdated.Broadcast();

		return true;
	}
}