#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Pal.h"
#include "PalStorageComponent.h"
#include "BreedingSlotWidget.generated.h"

class ABreedingFarm;

UCLASS()
class PALWORLD_STUDY_API UBreedingSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UImage* Img_PalIcon;

	// 이 슬롯에 의해 소환된 팰 액터 포인터
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Breeding")
	APal* SpawnedPalActor;

	// 팰이 스폰될 월드 좌표 (에디터에서 설정)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Breeding")
	FVector BreedingSpotLocation;

	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	void ClearSlot();

	// 이 슬롯이 부모 A인지 B인지 구분 (0 또는 1)
	UPROPERTY(EditAnywhere, Category = "Breeding")
	int32 SlotType;

	// 현재 월드에 있는 브리딩 팜 액터 참조
	UPROPERTY(BlueprintReadWrite, Category = "Breeding")
	ABreedingFarm* TargetFarm;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UPalStorageComponent* SourceStorageComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SourceSlotIndex;
};