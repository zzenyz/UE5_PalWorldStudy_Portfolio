#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Pal.h"
#include "PalStorageComponent.h"
#include "BreedingFarm.generated.h"

UCLASS()
class PALWORLD_STUDY_API ABreedingFarm : public AActor
{
	GENERATED_BODY()

public:
	ABreedingFarm();

protected:
	virtual void BeginPlay() override;

public:

	UFUNCTION()
	void UpdateParentSpawns();

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* FarmMesh;
	UPROPERTY(VisibleAnywhere, Category = "SpawnPoints")
	USceneComponent* ParentASpawnPoint;
	UPROPERTY(VisibleAnywhere, Category = "SpawnPoints")
	USceneComponent* ParentBSpawnPoint;
	UPROPERTY(VisibleAnywhere, Category = "SpawnPoints")
	USceneComponent* EggSpawnPoint;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Storage")
	UPalStorageComponent* FarmStorageComponent;

	UFUNCTION(BlueprintCallable, Category = "Breeding")
	void SetupBreedingUI(UUserWidget* InWidget);

	UPROPERTY(BlueprintReadWrite)
	APal* ParentA;
	UPROPERTY(BlueprintReadWrite)
	APal* ParentB;
	UPROPERTY(BlueprintReadOnly)
	bool bIsBreeding = false;
	UPROPERTY(BlueprintReadOnly)
	bool bIsEggReady = false;
	UPROPERTY(BlueprintReadOnly)
	class AEggActor* CurrentSpawnedEgg;
	UPROPERTY(EditAnywhere, Category = "Settings")
	TSubclassOf<class AEggActor> EggClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	UItemDataAsset* EggItemAsset;

	FTimerHandle BreedingTimerHandle;

	UFUNCTION(BlueprintCallable)
	void StartBreeding();

	void OnEggProduced();

	UFUNCTION(BlueprintCallable)
	void ClaimEgg(APawn* PlayerPawn);

	float GetRemainingTime() const;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	APal* SpawnedParentA;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	APal* SpawnedParentB;
};