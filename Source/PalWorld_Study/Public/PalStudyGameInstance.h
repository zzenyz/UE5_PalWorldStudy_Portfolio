#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "PalStudyGameInstance.generated.h"

class UDataTable;

UCLASS()
class PALWORLD_STUDY_API UPalStudyGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UPalStudyGameInstance();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SaveSystem")
	FString SaveSlotName = TEXT("PalStudy_SaveSlot_1");

	UFUNCTION(BlueprintCallable, Category = "SaveSystem")
	void SavePlayerData();

	UFUNCTION(BlueprintCallable, Category = "SaveSystem")
	void LoadPlayerData();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	UDataTable* ItemDataTable;

	UPROPERTY(BlueprintReadWrite, Category = "SaveSystem")
	bool bShouldLoadData = false;
};