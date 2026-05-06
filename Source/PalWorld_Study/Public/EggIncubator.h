#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemDataAsset.h" // FItemSlot 사용
#include "PalData.h"      // FPalData 사용
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
	// --- 컴포넌트 ---
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* DefaultSceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* IncubatorMesh;

	// 알이 놓일 위치 (에디터에서 'Egg' 소켓이나 컴포넌트로 배치)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* EggSpawnPoint;

	// --- 부화 데이터 ---
	// 부화기에 들어있는 알의 팰 데이터
	UPROPERTY(BlueprintReadOnly, Category = "Hatching")
	FPalData ContainedPalData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hatching")
	TSubclassOf<AActor> VisualEggClass; // BP_Egg 클래스

	UPROPERTY(BlueprintReadOnly, Category = "Hatching")
	AActor* SpawnedVisualEgg;

	UPROPERTY(BlueprintReadOnly, Category = "Hatching")
	bool bHasEgg = false;

	UPROPERTY(BlueprintReadOnly, Category = "Hatching")
	bool bIsHatched = false;

	UPROPERTY(EditAnywhere, Category = "Hatching")
	float HatchingTime = 10.0f; // 부화 시간 (초)

	FTimerHandle HatchTimerHandle;

	// --- 주요 함수 ---
	// UI에서 알을 넣을 때 호출
	UFUNCTION(BlueprintCallable, Category = "Incubator")
	void StartHatching(UInventoryComponent* PlayerInventory, int32 EggSlotIndex);

	// 부화 완료 시 실행
	void OnHatchingComplete();

	// 부화된 팰을 인벤토리로 가져가기
	UFUNCTION(BlueprintCallable, Category = "Hatching")
	void ClaimPal(APawn* PlayerPawn);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UInventoryComponent* IncubatorInventory;

	// 인벤토리에 변화가 생기면(알이 들어오거나 빠지면) 실행될 함수
	UFUNCTION()
	void OnIncubatorItemChanged();
};