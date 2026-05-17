#pragma once

#include "CoreMinimal.h"
#include "InventoryWidget.h"
#include "Components/UniformGridPanel.h" 
#include "SpawnerInventoryWidget.generated.h"

UCLASS()
class PALWORLD_STUDY_API USpawnerInventoryWidget : public UInventoryWidget
{
	GENERATED_BODY()

public:
	USpawnerInventoryWidget(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "Setup")
	void SetupSpawnerInfo(UInventoryComponent* InInventory, UPalStorageComponent* InStorage, UPalStorageComponent* InActiveStorage);

	virtual void RefreshAllViews() override;

protected:
	UPROPERTY(meta = (BindWidget))
	UUniformGridPanel* Grid_BaseItems;

	UPROPERTY(meta = (BindWidget)) UScrollBox* SB_BaseStoredPals;
	UPROPERTY(meta = (BindWidget)) UScrollBox* SB_BaseSpawnedPals;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	int32 ColumnsPerRow = 4;

	TWeakObjectPtr<UInventoryComponent> BaseInventoryComp;
	TWeakObjectPtr<UPalStorageComponent> BaseStorageComp;
	TWeakObjectPtr<UPalStorageComponent> BaseActiveComp;

	UFUNCTION() void RefreshBaseInventory();
	UFUNCTION() void RefreshBasePals();
};