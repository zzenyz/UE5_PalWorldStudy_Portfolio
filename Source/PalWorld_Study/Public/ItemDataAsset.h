// Fill out your copyright notice in the Description page of Project Settings.

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
    // 이 줄에 연결할 실제 아이템 에셋
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

    // 이 슬롯에 어떤 아이템이 들어있는지 (비어있을 수도 있음)
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UItemDataAsset* ItemAsset;

    // 이 슬롯에 아이템이 몇 개 들어있는지
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
    // 아이템의 고유 ID (내부 식별용)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data")
    FName ItemID;

    // UI에 표시될 이름
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data")
    FText DisplayName;

    // UI에 표시될 아이콘
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data")
    UTexture2D* Icon;

    // 한 슬롯에 최대로 쌓을 수 있는 개수
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data")
    int32 MaxStackSize = 99;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    float Weight = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    TSubclassOf<AActor> ItemBlueprint;
};
