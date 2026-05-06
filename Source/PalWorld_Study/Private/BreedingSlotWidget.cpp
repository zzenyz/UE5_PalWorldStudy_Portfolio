#include "BreedingSlotWidget.h"
#include "PalDragDropOp.h"
#include "Components/Image.h"
#include "BreedingFarm.h"

bool UBreedingSlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	UPalDragDropOp* PalOp = Cast<UPalDragDropOp>(InOperation);

	// 유효성 검사
	if (!PalOp || !TargetFarm || !TargetFarm->FarmStorageComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("[BreedSlot] Critical Error: Drop Failed!"));
		return false;
	}

	// 1. 데이터 가져오기
	UPalStorageComponent* SourceStorage = PalOp->SourceStorageComponent;
	int32 SourceIndex = PalOp->SourceIndex; // [수정] .h와 이름 통일
	int32 DestIndex = SlotType;

	// 2. Swap 실행
	if (SourceStorage)
	{
		bool bSwapSuccess = SourceStorage->SwapPals(SourceIndex, TargetFarm->FarmStorageComponent, DestIndex);
		if (!bSwapSuccess) return false;
	}

	// 3. 변수 정의 (if문 밖에서 사용할 수 있도록 선언)
	FPalData NewData = TargetFarm->FarmStorageComponent->StoredPalsData[DestIndex];
	UWorld* World = GetWorld();

	// 4. UI 이미지 업데이트
	if (Img_PalIcon && NewData.PalIcon.IsValid())
	{
		UTexture2D* LoadedIcon = NewData.PalIcon.LoadSynchronous();
		if (LoadedIcon)
		{
			Img_PalIcon->SetBrushFromTexture(LoadedIcon);
			Img_PalIcon->SetOpacity(1.0f);
		}
	}

	// 5. 월드 팰 스폰
	if (World && TargetFarm)
	{
		FTransform SpawnTransform = (SlotType == 0) ?
			TargetFarm->ParentASpawnPoint->GetComponentTransform() :
			TargetFarm->ParentBSpawnPoint->GetComponentTransform();

		if (SpawnedPalActor) SpawnedPalActor->Destroy();

		SpawnedPalActor = World->SpawnActor<APal>(APal::StaticClass(), SpawnTransform);
		if (SpawnedPalActor)
		{
			SpawnedPalActor->PalData = NewData;
			SpawnedPalActor->OnUpdateColor();
			SpawnedPalActor->AIState = 5; // Breeding 상태

			// [추가] 번식장 액터의 부모 포인터 업데이트
			if (SlotType == 0) TargetFarm->ParentA = SpawnedPalActor;
			else TargetFarm->ParentB = SpawnedPalActor;

			// 자동으로 번식 체크 시작
			TargetFarm->StartBreeding();
		}
	}

	return true;
}

void UBreedingSlotWidget::ClearSlot()
{
	if (SpawnedPalActor)
	{
		SpawnedPalActor->Destroy();
		SpawnedPalActor = nullptr;
	}
	if (Img_PalIcon) Img_PalIcon->SetOpacity(0.0f);
}