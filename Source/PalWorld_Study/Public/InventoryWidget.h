// InventoryWidget.h
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

	// ✅ 이 함수를 BP에서 "위젯을 켤 때(Visible)"마다 호출하세요!
	UFUNCTION(BlueprintCallable, Category = "UI")
	virtual void RefreshAllViews();

protected:
	// --- [공통] 플레이어 UI 요소 ---
	UPROPERTY(meta = (BindWidget))
	UUniformGridPanel* Grid_PlayerItems;

	const int32 ColumnsPerRow = 4;

	UPROPERTY(meta = (BindWidget))
	UPanelWidget* SB_PlayerPals;

	// --- [공통] 데이터 컴포넌트 ---
	TWeakObjectPtr<UInventoryComponent> PlayerInventoryComp;
	TWeakObjectPtr<UPalStorageComponent> PlayerPalStorageComp;

	// --- [공통] 갱신 함수 ---
	UFUNCTION()
	void RefreshPlayerInventory();

	UFUNCTION()
	void RefreshPlayerPals();

	// --- 슬롯 클래스 (BP 설정) ---
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> ItemSlotClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> PalSlotClass;

protected:
	// 블루프린트에 배치한 버튼 이름이 'SaveButton'이어야 합니다.
	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_SaveGame;

	// 실제 저장 로직을 실행할 함수
	UFUNCTION()
	void OnSaveButtonClicked();
};