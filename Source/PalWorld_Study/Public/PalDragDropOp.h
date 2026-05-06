#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "PalData.h" // FPalData가 정의된 헤더
#include "PalDragDropOp.generated.h"

UCLASS()
class PALWORLD_STUDY_API UPalDragDropOp : public UDragDropOperation
{
	GENERATED_BODY()

public:
	// 드래그 중인 팰의 실제 데이터
	UPROPERTY(BlueprintReadWrite, Category = "Breeding")
	FPalData StoredPalData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UPalStorageComponent* SourceStorageComponent;

	// 원래 인벤토리 슬롯 인덱스 (필요 시)
	UPROPERTY(BlueprintReadWrite, Category = "Breeding")
	int32 SourceIndex;
};