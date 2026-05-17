#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ItemDataAsset.h"
#include "InventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryUpdated);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PALWORLD_STUDY_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInventoryComponent();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 Capacity = 20;

public:
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnInventoryUpdated OnInventoryUpdated;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	TArray<FItemSlot> Slots;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool AddItem(UItemDataAsset* ItemAsset, int32 Quantity);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool SwapItems(int32 SourceIndex, UInventoryComponent* DestinationInventory, int32 DestinationIndex);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 GetCapacity() const { return Capacity; }
};