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

	TArray<TWeakObjectPtr<APal>> SpawnedPals;

	UFUNCTION(BlueprintCallable, Category = "Pal Storage")
	bool AddPal(APal* PalToAdd, UPalStorageComponent* OverflowStorage = nullptr);

	UFUNCTION(BlueprintCallable, Category = "Pal Storage")
	bool RemovePalByIndex(int32 Index);

	UFUNCTION(BlueprintCallable, Category = "Pal Storage")
	bool SwapPals(int32 SourceIndex, UPalStorageComponent* DestinationStorage, int32 DestinationIndex);

	UFUNCTION(BlueprintCallable, Category = "Pal Storage")
	APal* SpawnPal(int32 PalIndex, const FTransform& SpawnTransform, AActor* Spawner);

	UFUNCTION(BlueprintCallable, Category = "Pal Storage")
	bool UnsummonPalByIndex(int32 PalIndex);

	UFUNCTION(BlueprintCallable, Category = "Pal Storage")
	bool UnsummonAllPals();

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};