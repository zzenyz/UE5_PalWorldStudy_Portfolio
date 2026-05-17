#include "PalMainMenuGameMode.h"
#include "MainWidget.h"
#include "Blueprint/UserWidget.h"

APalMainMenuGameMode::APalMainMenuGameMode()
{
	DefaultPawnClass = nullptr;
}

void APalMainMenuGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (MainWidgetClass)
	{
		UMainWidget* MenuWidget = CreateWidget<UMainWidget>(GetWorld(), MainWidgetClass);
		if (MenuWidget)
		{
			MenuWidget->AddToViewport();
		}
	}
}