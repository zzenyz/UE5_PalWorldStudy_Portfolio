#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "PalData.h"
#include "PalStudySaveGame.generated.h"

USTRUCT(BlueprintType)
struct FSavedItemSlot
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere)
    FName ItemID;

    UPROPERTY(VisibleAnywhere)
    int32 Quantity;
};

UCLASS()
class UPalStudySaveGame : public USaveGame
{
    GENERATED_BODY()

public:
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SaveData")
    TArray<FSavedItemSlot> SavedInventory;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SaveData")
    TArray<FPalData> SavedPals;

    UPROPERTY()
    TArray<FSavedItemSlot> SpawnerInventory;

    UPROPERTY()
    TArray<FPalData> SpawnerPals;

    UPROPERTY()
    TArray<FPalData> SpawnerActivePals;
};