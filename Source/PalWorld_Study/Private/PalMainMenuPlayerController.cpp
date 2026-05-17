#include "PalMainMenuPlayerController.h"

void APalMainMenuPlayerController::BeginPlay()
{
	Super::BeginPlay();

	bShowMouseCursor = true;

	// 플레이어 조작을 막고 UI 전용 입력 모드로 전환
	FInputModeUIOnly InputMode;
	SetInputMode(InputMode);
}