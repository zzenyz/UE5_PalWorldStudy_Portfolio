// PalStorageComponent.h

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PalData.h"
#include "PalStorageComponent.generated.h"

class APal;
class UDataTable;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPalStorageUpdated);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PALWORLD_STUDY_API UPalStorageComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPalStorageComponent();

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnPalStorageUpdated OnPalStorageUpdated;

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pal Storage")
	bool bIsActiveInventory = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pal Storage")
	TArray<FPalData> StoredPalsData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pal Storage")
	int32 StorageCapacity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pal Storage")
	UDataTable* PalDataTable;

	/** [리팩토링 ✨] TMap 대신 1:1 대응하는 배열을 사용합니다. 블루프린트 노출 안함. */
	TArray<TWeakObjectPtr<APal>> SpawnedPals;


	/** 팰을 보관함에 추가합니다. */
	UFUNCTION(BlueprintCallable, Category = "Pal Storage")
	bool AddPal(APal* PalToAdd, UPalStorageComponent* OverflowStorage = nullptr);

	/** 인덱스로 팰 데이터를 제거합니다. (실제로는 '비움') */
	UFUNCTION(BlueprintCallable, Category = "Pal Storage")
	bool RemovePalByIndex(int32 Index);

	/** 팰을 교환합니다. (이동/맞교환 통합) */
	UFUNCTION(BlueprintCallable, Category = "Pal Storage")
	bool SwapPals(int32 SourceIndex, UPalStorageComponent* DestinationStorage, int32 DestinationIndex);

	/** 팰을 스폰합니다. */
	UFUNCTION(BlueprintCallable, Category = "Pal Storage")
	APal* SpawnPal(int32 PalIndex, const FTransform& SpawnTransform, AActor* Spawner);

	/** 인덱스로 팰을 소환 해제합니다. */
	UFUNCTION(BlueprintCallable, Category = "Pal Storage")
	bool UnsummonPalByIndex(int32 PalIndex);

	/** 모든 팰을 소환 해제합니다. */
	UFUNCTION(BlueprintCallable, Category = "Pal Storage")
	bool UnsummonAllPals();

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

};

