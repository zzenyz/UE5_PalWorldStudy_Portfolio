#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PalStorageComponent.h"
#include "PalSlotWidget.generated.h"

UCLASS()
class PALWORLD_STUDY_API UPalSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void SetPalSlotData(const FPalData& PalData);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = "true"))
	UPalStorageComponent* OwnerStorageComponent;

	UPROPERTY(BlueprintReadWrite, Meta = (ExposeOnSpawn = true), Category = "Data")
	int32 SlotIndex;

	UPROPERTY(BlueprintReadWrite, Category = "Breeding")
	class APal* SpawnedPalActor;
};