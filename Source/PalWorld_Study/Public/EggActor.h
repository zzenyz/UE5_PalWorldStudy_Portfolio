#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PalData.h"
#include "EggActor.generated.h"

UCLASS()
class PALWORLD_STUDY_API AEggActor : public AActor
{
	GENERATED_BODY()

public:
	AEggActor();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* DefaultSceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* EggMesh;

	// 교배 유전 정보 데이터
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Egg Data")
	FPalData StoredPalData;

	UFUNCTION(BlueprintCallable, Category = "Egg")
	void SetEggVisual(UStaticMesh* NewMesh);
};