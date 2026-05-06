// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PalWorld_StudyCharacter.h"
#include "Blueprint/UserWidget.h"
#include "PlayerSystem.generated.h"

class UInventoryComponent;
class UPalStorageComponent;
class APal; // 전방 선언 유지

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
	// ====== 팰 소환 상태를 관리하는 변수 ======

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Pal | Summon")
	int32 SelectedPalIndex; // UI에서 선택된 인덱스 (이건 유지)

	// 부모의 BeginPlay를 명시적으로 실행하기 위해 선언합니다.
	virtual void BeginPlay() override;

public:
	/** * [E키] 이 함수 하나로 호출하면 모든 상황을 처리합니다.
	 * 소환, 소환 해제, 교체를 알아서 판단합니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "Pal | Actions")
	void HandleSummonAction(int32 IndexToHandle);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<class UUserWidget> MainWidgetClass;

};
