#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "PalData.h" 
#include "PalDragDropOp.generated.h"

UCLASS()
class PALWORLD_STUDY_API UPalDragDropOp : public UDragDropOperation
{
	GENERATED_BODY()

public:
	// 드래그 중인 데이터 원본
	UPROPERTY(BlueprintReadWrite, Category = "DragDrop")
	FPalData StoredPalData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DragDrop")
	UPalStorageComponent* SourceStorageComponent;

	// 이동 전 슬롯 위치 기록
	UPROPERTY(BlueprintReadWrite, Category = "DragDrop")
	int32 SourceIndex;
};