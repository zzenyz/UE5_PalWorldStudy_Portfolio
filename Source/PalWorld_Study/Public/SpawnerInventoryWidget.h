#pragma once

#include "CoreMinimal.h"
#include "InventoryWidget.h"
#include "Components/UniformGridPanel.h" // 👈 Grid Panel 헤더 필수!
#include "SpawnerInventoryWidget.generated.h"

UCLASS()
class PALWORLD_STUDY_API USpawnerInventoryWidget : public UInventoryWidget
{
	GENERATED_BODY()

public:
	USpawnerInventoryWidget(const FObjectInitializer& ObjectInitializer);

	// ✅ BP에서 스포너를 열 때(Visible 직후) 이 함수를 부르면 Refresh까지 한방에 해결됩니다.
	UFUNCTION(BlueprintCallable, Category = "Setup")
	void SetupSpawnerInfo(UInventoryComponent* InInventory, UPalStorageComponent* InStorage, UPalStorageComponent* InActiveStorage);

	virtual void RefreshAllViews() override;

protected:
	// --- UI ---
	UPROPERTY(meta = (BindWidget))
	UUniformGridPanel* Grid_BaseItems; // 👈 스크롤박스 대신 그리드 패널!

	UPROPERTY(meta = (BindWidget)) UScrollBox* SB_BaseStoredPals;
	UPROPERTY(meta = (BindWidget)) UScrollBox* SB_BaseSpawnedPals;

	// --- 설정 변수 ---
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	int32 ColumnsPerRow = 4; // 👈 [추가] 가로로 몇 개 놓을지 변수 (기본 5개)

	// --- 데이터 ---
	TWeakObjectPtr<UInventoryComponent> BaseInventoryComp;
	TWeakObjectPtr<UPalStorageComponent> BaseStorageComp;
	TWeakObjectPtr<UPalStorageComponent> BaseActiveComp;

	// --- 갱신 함수 ---
	UFUNCTION() void RefreshBaseInventory();
	UFUNCTION() void RefreshBasePals();
};