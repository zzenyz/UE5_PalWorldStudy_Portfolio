#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PalStorageComponent.h" // FPalData 구조체 필요
#include "PalSlotWidget.generated.h"

UCLASS()
class PALWORLD_STUDY_API UPalSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// ✅ BP에 있는 "PalData" 변수에 값을 넣어줄 함수 (BP에서 구현)
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void SetPalSlotData(const FPalData& PalData);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = "true"))
	UPalStorageComponent* OwnerStorageComponent;

	// ✅ [추가] 몇 번째 칸인지 저장할 변수 (이미 있다면 패스)
	UPROPERTY(BlueprintReadWrite, Meta = (ExposeOnSpawn = true), Category = "Data")
	int32 SlotIndex;

	UPROPERTY(BlueprintReadWrite, Category = "Breeding")
	class APal* SpawnedPalActor;

};