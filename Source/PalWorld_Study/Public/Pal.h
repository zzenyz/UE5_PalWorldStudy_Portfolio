#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InventoryComponent.h"
#include "AIController.h"
#include "PalData.h"
#include "Pal.generated.h"

UCLASS()
class PALWORLD_STUDY_API APal : public ACharacter
{
	GENERATED_BODY()

public:
	APal();
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pal|Data")
	FPalData PalData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	uint8 AIState;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Pal|Work")
	UItemDataAsset* CarriedItemData;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Pal|Work")
	int32 CarriedItemQuantity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pal|Work")
	UStaticMeshComponent* CarriedItemMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pal|State")
	bool bIsAttacking = false;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Pal|State")
	AActor* SpawnerActor;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Pal|Stats")
	float FinalMaxHP;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Pal|Stats")
	float FinalAttack;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Pal|Stats")
	float FinalDefense;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pal|Level")
	float BaseXP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pal|Level")
	float Exponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pal|AI")
	TSubclassOf<AAIController> PalAIControllerClass;

	class ABuilding* OwnerBase;

	UFUNCTION(BlueprintCallable, Category = "Pal")
	void RefreshPalState();

	UFUNCTION(BlueprintCallable)
	float GetMaxHP() const { return FinalMaxHP; }
	UFUNCTION(BlueprintCallable)
	float GetAttack() const { return FinalAttack; }
	UFUNCTION(BlueprintCallable)
	float GetDefense() const { return FinalDefense; }

	UFUNCTION(BlueprintCallable)
	static const TMap<EPalTraitType, FPalTrait>& GetTraitMap();

	UFUNCTION(BlueprintCallable, Category = "Breeding")
	static FPalData Breed(const FPalData& ParentA, const FPalData& ParentB);

	UFUNCTION(BlueprintCallable)
	void RecalculateStats();

	UFUNCTION(BlueprintImplementableEvent, Category = "Pal | Visuals")
	void OnUpdateColor();

	UFUNCTION(BlueprintCallable)
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintCallable)
	float GetXPToNextLevel() const;
	UFUNCTION(BlueprintCallable)
	void GainExperience(float Amount);
	UFUNCTION(BlueprintCallable)
	void CheckLevelUp();
	UFUNCTION(BlueprintCallable)
	void LevelUp();

	UFUNCTION(BlueprintImplementableEvent, Category = "Attack")
	void ExecuteBPAttack();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Pal Data")
	class UDataTable* PalDataTable;

	UPROPERTY(EditDefaultsOnly, Category = "Pal Data")
	FName PalSpeciesID;

public:
	UFUNCTION()
	void HandleMasterCombat(AActor* TargetEnemy);

	UFUNCTION(BlueprintCallable, Category = "Pal|Combat")
	void SetMaster(AActor* NewMaster);
};