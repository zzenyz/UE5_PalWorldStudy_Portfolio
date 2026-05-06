// Fill out your copyright notice in the Description page of Project Settings.

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
    // 1. 플레이어 인벤토리 저장용 배열
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SaveData")
    TArray<FSavedItemSlot> SavedInventory;

    // 2. 팰 보관함 저장용 배열 (FPalData 구조체를 그대로 배열로 저장 가능!)
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SaveData")
    TArray<FPalData> SavedPals;

    UPROPERTY()
    TArray<FSavedItemSlot> SpawnerInventory; // 스포너 아이템

    UPROPERTY()
    TArray<FPalData> SpawnerPals; // 스포너 보관함 팰

    UPROPERTY()
    TArray<FPalData> SpawnerActivePals;
	
};
