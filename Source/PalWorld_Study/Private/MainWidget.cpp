#include "MainWidget.h"
#include "PalStudyGameInstance.h"

void UMainWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_NewGame)
		Btn_NewGame->OnClicked.AddDynamic(this, &UMainWidget::OnNewGameClicked);

	if (Btn_LoadGame)
		Btn_LoadGame->OnClicked.AddDynamic(this, &UMainWidget::OnLoadGameClicked);
}

void UMainWidget::OnNewGameClicked()
{
    // 1. 위젯 제거
    RemoveFromParent();

    // 2. 마우스 숨기고 게임 모드로 복귀
    APlayerController* PC = GetOwningPlayer();
    if (PC)
    {
        PC->bShowMouseCursor = false;
        PC->SetInputMode(FInputModeGameOnly());
    }
    // 새 게임이므로 추가 로드 없이 이대로 플레이 시작!
}

void UMainWidget::OnLoadGameClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("CHECK 1: Load Button Clicked!"));

	if (UPalStudyGameInstance* GI = Cast<UPalStudyGameInstance>(GetGameInstance()))
	{
		UE_LOG(LogTemp, Warning, TEXT("CHECK 2: GameInstance Found. Calling LoadPlayerData..."));

		GI->LoadPlayerData();

		// 위젯 닫고 컨트롤러 돌려놓기
		RemoveFromParent();
		APlayerController* PC = GetOwningPlayer();
		if (PC)
		{
			PC->bShowMouseCursor = false;
			PC->SetInputMode(FInputModeGameOnly());
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("CHECK 2 ERROR: GameInstance is NOT PalStudyGameInstance!"));
	}
}