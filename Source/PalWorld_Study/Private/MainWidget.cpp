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
	RemoveFromParent();

	APlayerController* PC = GetOwningPlayer();
	if (PC)
	{
		PC->bShowMouseCursor = false;
		PC->SetInputMode(FInputModeGameOnly());
	}
}

void UMainWidget::OnLoadGameClicked()
{
	if (UPalStudyGameInstance* GI = Cast<UPalStudyGameInstance>(GetGameInstance()))
	{
		GI->LoadPlayerData();

		RemoveFromParent();
		APlayerController* PC = GetOwningPlayer();
		if (PC)
		{
			PC->bShowMouseCursor = false;
			PC->SetInputMode(FInputModeGameOnly());
		}
	}
}