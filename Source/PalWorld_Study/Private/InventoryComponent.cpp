// Fill out your copyright notice in the Description page of Project Settings.

#include "InventoryComponent.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.
	// You can turn these features off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// 컴포넌트가 생성될 때, Capacity(용량)만큼 인벤토리 칸을 미리 만들어둡니다.
	Slots.SetNum(Capacity);
}

bool UInventoryComponent::AddItem(UItemDataAsset* ItemAsset, int32 Quantity)
{
	if (!ItemAsset || Quantity <= 0)
	{
		return false;
	}

	int32 QuantityToAdd = Quantity;

	// 1단계: 이미 있는 아이템 슬롯을 찾아 개수를 채웁니다 (스태킹).
	for (FItemSlot& Slot : Slots)
	{
		if (Slot.ItemAsset == ItemAsset && Slot.Quantity < ItemAsset->MaxStackSize)
		{
			int32 CanAddAmount = ItemAsset->MaxStackSize - Slot.Quantity;
			int32 AddAmount = FMath::Min(QuantityToAdd, CanAddAmount);
			Slot.Quantity += AddAmount;
			QuantityToAdd -= AddAmount;

			// 다 넣었으면 성공!
			if (QuantityToAdd <= 0)
			{
				// ✅ [수정] 데이터가 변했으니 UI 갱신 알림
				if (OnInventoryUpdated.IsBound()) OnInventoryUpdated.Broadcast();
				return true;
			}
		}
	}

	// 2단계: 남은 아이템을 빈 슬롯에 새로 추가합니다.
	for (FItemSlot& Slot : Slots)
	{
		if (Slot.ItemAsset == nullptr)
		{
			int32 AddAmount = FMath::Min(QuantityToAdd, ItemAsset->MaxStackSize);
			Slot.ItemAsset = ItemAsset;
			Slot.Quantity = AddAmount;
			QuantityToAdd -= AddAmount;

			// 다 넣었으면 성공!
			if (QuantityToAdd <= 0)
			{
				// ✅ [수정] 데이터가 변했으니 UI 갱신 알림
				if (OnInventoryUpdated.IsBound()) OnInventoryUpdated.Broadcast();
				return true;
			}
		}
	}

	// (혹시 일부만 들어가고 남았더라도, 인벤토리에 조금이라도 변화가 있었으면 방송해줍니다)
	if (QuantityToAdd < Quantity)
	{
		if (OnInventoryUpdated.IsBound()) OnInventoryUpdated.Broadcast();
	}

	return false; // 인벤토리가 가득 차서 (전부 다 넣지는 못해서) 실패함.
}

// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	// 필요한 초기화 로직이 있다면 여기에 작성
}

// Called every frame
void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	// 틱 로직이 필요하다면 여기에 작성
}

bool UInventoryComponent::SwapItems(int32 SourceIndex, UInventoryComponent* DestinationInventory, int32 DestinationIndex)
{
	// 1. 안전장치: 필수 정보가 없거나 출발지 인덱스가 잘못되면 실패
	if (!DestinationInventory || !this->Slots.IsValidIndex(SourceIndex))
	{
		UE_LOG(LogTemp, Warning, TEXT("SwapItems Failed: Invalid Destination Inventory or Source Index (%d)"), SourceIndex);
		return false;
	}

	// 참조를 사용하면 코드가 더 간결해집니다.
	FItemSlot& SourceSlot = this->Slots[SourceIndex];

	// 2. 같은 인벤토리 내에서 교환하는 경우
	if (this == DestinationInventory)
	{
		// 목적지 인덱스 유효성 검사
		if (!this->Slots.IsValidIndex(DestinationIndex))
		{
			UE_LOG(LogTemp, Warning, TEXT("SwapItems Failed (Same Inv): Invalid Destination Index (%d)"), DestinationIndex);
			return false;
		}

		FItemSlot& DestinationSlot = this->Slots[DestinationIndex];

		// --- 스태킹 로직 시작 (같은 인벤토리) ---
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

				// ✅ [수정] 내부 변경 알림 (주석 해제)
				if (OnInventoryUpdated.IsBound()) OnInventoryUpdated.Broadcast();
				return true;
			}
		}
		// --- 스태킹 로직 끝 ---

		// 단순 교환 수행
		this->Slots.Swap(SourceIndex, DestinationIndex);

		// ✅ [수정] 교환 후 알림 (주석 해제)
		if (OnInventoryUpdated.IsBound()) OnInventoryUpdated.Broadcast();
		return true;
	}

	// 3. 다른 인벤토리와 교환하는 경우 (예: 플레이어 <-> 상자)
	else
	{
		// 목적지 인덱스 유효성 검사
		if (!DestinationInventory->Slots.IsValidIndex(DestinationIndex))
		{
			UE_LOG(LogTemp, Warning, TEXT("SwapItems Failed (Different Inv): Invalid Destination Index (%d)"), DestinationIndex);
			return false;
		}

		FItemSlot& DestinationSlot = DestinationInventory->Slots[DestinationIndex];

		// --- 스태킹 로직 시작 (다른 인벤토리) ---
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

				// ✅ [수정] 양쪽 다 방송해야 함! (주석 해제)
				if (this->OnInventoryUpdated.IsBound())
					this->OnInventoryUpdated.Broadcast();

				if (DestinationInventory->OnInventoryUpdated.IsBound())
					DestinationInventory->OnInventoryUpdated.Broadcast();

				return true;
			}
		}
		// --- 스태킹 로직 끝 ---

		// 스태킹 조건이 아니면 슬롯 내용 전체를 교환
		Swap(SourceSlot, DestinationSlot);

		// ✅ [수정] 양쪽 다 방송! (주석 해제)
		if (this->OnInventoryUpdated.IsBound())
			this->OnInventoryUpdated.Broadcast();

		if (DestinationInventory->OnInventoryUpdated.IsBound())
			DestinationInventory->OnInventoryUpdated.Broadcast();

		return true;
	}
}
