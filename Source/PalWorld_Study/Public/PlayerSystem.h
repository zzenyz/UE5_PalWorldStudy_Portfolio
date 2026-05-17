#pragma once

#include "CoreMinimal.h"
#include "PalWorld_StudyCharacter.h"
#include "Blueprint/UserWidget.h"
#include "PlayerSystem.generated.h"

class UInventoryComponent;
class UPalStorageComponent;
class APal;

UCLASS()
class PALWORLD_STUDY_API APlayerSystem : public APalWorld_StudyCharacter
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UInventoryComponent* Inventory;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UPalStorageComponent* PalInventory;

	APlayerSystem();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Pal | Summon")
	int32 SelectedPalIndex;

	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable, Category = "Pal | Actions")
	void HandleSummonAction(int32 IndexToHandle);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<class UUserWidget> MainWidgetClass;
};