// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ItemDataAsset.h"
#include "InventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryUpdated);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PALWORLD_STUDY_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventoryComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// 인벤토리의 최대 슬롯 개수
	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 Capacity = 20;

public:	
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnInventoryUpdated OnInventoryUpdated;

	// Called every frame
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
