#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ScrollBox.h"
#include "Components/UniformGridPanel.h"
#include "Components/PanelWidget.h"
#include "InventoryComponent.h"
#include "PalStorageComponent.h" 
#include "Components/Button.h"
#include "InventoryWidget.generated.h"

UCLASS()
class PALWORLD_STUDY_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UInventoryWidget(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable, Category = "UI")
	virtual void RefreshAllViews();

protected:
	UPROPERTY(meta = (BindWidget))
	UUniformGridPanel* Grid_PlayerItems;

	const int32 ColumnsPerRow = 4;

	UPROPERTY(meta = (BindWidget))
	UPanelWidget* SB_PlayerPals;

	TWeakObjectPtr<UInventoryComponent> PlayerInventoryComp;
	TWeakObjectPtr<UPalStorageComponent> PlayerPalStorageComp;

	UFUNCTION()
	void RefreshPlayerInventory();

	UFUNCTION()
	void RefreshPlayerPals();

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> ItemSlotClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> PalSlotClass;

protected:
	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_SaveGame;

	UFUNCTION()
	void OnSaveButtonClicked();
};