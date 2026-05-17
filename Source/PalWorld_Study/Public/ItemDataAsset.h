#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/DataTable.h"
#include "PalData.h"
#include "ItemDataAsset.generated.h"

USTRUCT(BlueprintType)
struct FItemDataRow : public FTableRowBase
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Data")
    UItemDataAsset* ItemAsset;
};

USTRUCT(BlueprintType)
struct FItemSlot
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemData")
    FName ItemID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UItemDataAsset* ItemAsset;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Quantity;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemData")
    FPalData StoredPalData;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemData")
    bool bHasPalData;

    FItemSlot()
    {
        ItemAsset = nullptr;
        Quantity = 0;
    }
};

UCLASS(BlueprintType)
class PALWORLD_STUDY_API UItemDataAsset : public UDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data")
    FName ItemID;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data")
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data")
    UTexture2D* Icon;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data")
    int32 MaxStackSize = 99;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    float Weight = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    TSubclassOf<AActor> ItemBlueprint;
};