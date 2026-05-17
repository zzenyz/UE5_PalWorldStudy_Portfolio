#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryComponent.h"
#include "IncubatorWidget.generated.h"

class AEggIncubator;
class UProgressBar;
class UButton;
class UTextBlock;
class UUniformGridPanel;
class UVerticalBox;

UCLASS()
class PALWORLD_STUDY_API UIncubatorWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(BlueprintReadWrite, Category = "Incubator", meta = (ExposeOnSpawn = true))
	AEggIncubator* TargetIncubator;

	TWeakObjectPtr<UInventoryComponent> PlayerInventoryComp;

protected:
	UPROPERTY(meta = (BindWidget))
	UProgressBar* PB_HatchingProgress;
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Action;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_BtnAction;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_Timer;

	UPROPERTY(meta = (BindWidget))
	UUniformGridPanel* Grid_PlayerItems;

	UPROPERTY(meta = (BindWidget))
	UVerticalBox* Box_IncubatorSlot;

	UFUNCTION()
	void RefreshPlayerInventory();

	UFUNCTION()
	void RefreshIncubatorSlot();

	UFUNCTION()
	void OnActionClicked();

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> ItemSlotClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	int32 ColumnsPerRow = 5;

public:
	UFUNCTION(BlueprintCallable)
	void SetTargetIncubator(class AEggIncubator* InIncubator);
};