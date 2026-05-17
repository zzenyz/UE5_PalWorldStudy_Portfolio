#include "PalMainMenuWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PalStudyGameInstance.h"

void UPalMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_NewGame) Btn_NewGame->OnClicked.AddDynamic(this, &UPalMainMenuWidget::OnNewGameClicked);
	if (Btn_LoadGame) Btn_LoadGame->OnClicked.AddDynamic(this, &UPalMainMenuWidget::OnLoadGameClicked);
	if (Btn_Exit) Btn_Exit->OnClicked.AddDynamic(this, &UPalMainMenuWidget::OnExitClicked);
}

void UPalMainMenuWidget::OnNewGameClicked()
{
	if (UPalStudyGameInstance* GI = Cast<UPalStudyGameInstance>(UGameplayStatics::GetGameInstance(this)))
	{
		GI->bShouldLoadData = false;
	}

	UGameplayStatics::OpenLevel(this, FName("GameMap"));
}

void UPalMainMenuWidget::OnLoadGameClicked()
{
	if (UPalStudyGameInstance* GI = Cast<UPalStudyGameInstance>(UGameplayStatics::GetGameInstance(this)))
	{
		if (UGameplayStatics::DoesSaveGameExist(GI->SaveSlotName, 0))
		{
			GI->bShouldLoadData = true;
			UGameplayStatics::OpenLevel(this, FName("GameMap"));
		}

	}
}

void UPalMainMenuWidget::OnExitClicked()
{
	UKismetSystemLibrary::QuitGame(this, nullptr, EQuitPreference::Quit, false);
}