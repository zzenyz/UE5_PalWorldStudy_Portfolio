// ItemSlotWidget.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryComponent.h" // FItemSlot 구조체 필요
#include "ItemSlotWidget.generated.h"

UCLASS()
class PALWORLD_STUDY_API UItemSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Meta = (ExposeOnSpawn = true), Category = "Data")
	UInventoryComponent* OwnerInvenComponent;

	// ✅ [추가] 몇 번째 칸인지
	UPROPERTY(BlueprintReadWrite, Meta = (ExposeOnSpawn = true), Category = "Data")
	int32 SlotIndex;

	// [오류 해결 2] CPP에서 호출하려는 함수 선언 추가
	// (구현은 CPP 파일에 하거나, 단순 데이터 전달용이면 여기서 인라인으로 처리해도 됩니다)
	UFUNCTION(BlueprintCallable, Category = "UI")
	void SetItemSlotData(const FItemSlot& SlotData);

	// 기존에 만드신 BP 이벤트 (이건 그대로 두셔도 됩니다)
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void UpdateSlotUI(const FItemSlot& SlotData);
};