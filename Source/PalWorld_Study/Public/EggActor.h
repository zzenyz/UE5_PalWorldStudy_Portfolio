#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PalData.h" // FPalData 사용을 위해 포함
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
	// --- 컴포넌트 ---
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* DefaultSceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* EggMesh;

	// --- 데이터 ---
	// 부모로부터 물려받은 유전 정보가 여기에 담깁니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Egg Data")
	FPalData StoredPalData;

	// 에디터에서 알의 종류(불, 물 등)에 따라 메쉬를 다르게 보여주고 싶을 때 사용
	UFUNCTION(BlueprintCallable, Category = "Egg")
	void SetEggVisual(UStaticMesh* NewMesh);
};