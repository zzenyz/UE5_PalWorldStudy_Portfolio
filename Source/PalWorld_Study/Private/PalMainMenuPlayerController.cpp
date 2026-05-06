#include "PalMainMenuPlayerController.h"

void APalMainMenuPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// 1. 마우스 커서를 화면에 보이게 합니다.
	bShowMouseCursor = true;

	// 2. 플레이어의 입력을 UI 전용으로 제한합니다. (캐릭터 조작 방지)
	FInputModeUIOnly InputMode;
	SetInputMode(InputMode);
}