#include "ItemSlotWidget.h"

void UItemSlotWidget::SetItemSlotData(const FItemSlot& SlotData)
{
    // 데이터를 받아서 BP 이벤트(UpdateSlotUI)를 실행
    UpdateSlotUI(SlotData);
}