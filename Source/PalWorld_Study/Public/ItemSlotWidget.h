#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryComponent.h"
#include "ItemSlotWidget.generated.h"

UCLASS()
class PALWORLD_STUDY_API UItemSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Meta = (ExposeOnSpawn = true), Category = "Data")
	UInventoryComponent* OwnerInvenComponent;

	UPROPERTY(BlueprintReadWrite, Meta = (ExposeOnSpawn = true), Category = "Data")
	int32 SlotIndex;

	UFUNCTION(BlueprintCallable, Category = "UI")
	void SetItemSlotData(const FItemSlot& SlotData);

	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void UpdateSlotUI(const FItemSlot& SlotData);
};