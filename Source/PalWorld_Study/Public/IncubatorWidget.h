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

	// [핵심] 스포너처럼 플레이어 인벤토리를 동적 할당할 그리드
	UPROPERTY(meta = (BindWidget))
	UUniformGridPanel* Grid_PlayerItems;

	// [핵심] 브리딩처럼 부화기 슬롯을 동적 할당할 박스
	UPROPERTY(meta = (BindWidget))
	UVerticalBox* Box_IncubatorSlot;

	UFUNCTION()
	void RefreshPlayerInventory();

	UFUNCTION()
	void RefreshIncubatorSlot();

	UFUNCTION()
	void OnActionClicked();

	// 생성할 아이템 슬롯 클래스 (WBP_ItemSlot)
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> ItemSlotClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	int32 ColumnsPerRow = 5;

public:
	// 브리딩의 SetTargetFarm과 완벽히 동일한 역할
	UFUNCTION(BlueprintCallable)
	void SetTargetIncubator(class AEggIncubator* InIncubator);
};