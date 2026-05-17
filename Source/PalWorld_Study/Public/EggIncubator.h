#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemDataAsset.h"
#include "PalData.h"
#include "EggIncubator.generated.h"

UCLASS()
class PALWORLD_STUDY_API AEggIncubator : public AActor
{
	GENERATED_BODY()

public:
	AEggIncubator();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* DefaultSceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* IncubatorMesh;

	// 알 배치 소켓 위치
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* EggSpawnPoint;

	// 부화기 내부 데이터
	UPROPERTY(BlueprintReadOnly, Category = "Hatching")
	FPalData ContainedPalData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hatching")
	TSubclassOf<AActor> VisualEggClass;

	UPROPERTY(BlueprintReadOnly, Category = "Hatching")
	AActor* SpawnedVisualEgg;

	UPROPERTY(BlueprintReadOnly, Category = "Hatching")
	bool bHasEgg = false;

	UPROPERTY(BlueprintReadOnly, Category = "Hatching")
	bool bIsHatched = false;

	UPROPERTY(EditAnywhere, Category = "Hatching")
	float HatchingTime = 10.0f; // 부화 시간(초)

	FTimerHandle HatchTimerHandle;

	UFUNCTION(BlueprintCallable, Category = "Incubator")
	void StartHatching(UInventoryComponent* PlayerInventory, int32 EggSlotIndex);

	void OnHatchingComplete();

	UFUNCTION(BlueprintCallable, Category = "Hatching")
	void ClaimPal(APawn* PlayerPawn);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UInventoryComponent* IncubatorInventory;

	// 아이템 변동 감지용 델리게이트
	UFUNCTION()
	void OnIncubatorItemChanged();
};