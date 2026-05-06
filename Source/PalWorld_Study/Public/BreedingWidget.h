#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PalStorageComponent.h"
// UPalSlotWidget.h 포함을 지우셔도 됩니다 (전방 선언으로 대체 가능)
#include "BreedingWidget.generated.h"

class ABreedingFarm;
class UProgressBar;
class UButton;
class UTextBlock;
class UVerticalBox;

UCLASS()
class PALWORLD_STUDY_API UBreedingWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	UPROPERTY(BlueprintReadWrite, Category = "Breeding", meta = (ExposeOnSpawn = true))
	ABreedingFarm* TargetFarm;

	TWeakObjectPtr<UPalStorageComponent> PlayerPalStorageComp;

protected:
	UPROPERTY(meta = (BindWidget))
	UProgressBar* PB_EggProgress;
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Breed;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_BreedBtn;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_Status;
	UPROPERTY(meta = (BindWidget))
	UVerticalBox* SB_PlayerPals;

	UPROPERTY(meta = (BindWidget))
	UVerticalBox* SB_FarmParents;

	UFUNCTION()
	void RefreshPalList();

	UFUNCTION()
	void OnBreedButtonClicked();

	// [핵심] 농장의 스토리지가 바뀔 때마다 팰을 소환하고 슬롯 UI를 갱신할 델리게이트 함수
	UFUNCTION()
	void OnFarmStorageUpdated();

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> PalSlotClass;

public:

	UFUNCTION(BlueprintCallable)
	void SetTargetFarm(class ABreedingFarm* InFarm);
};