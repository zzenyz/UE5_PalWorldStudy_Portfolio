#include "PalMainMenuGameMode.h"
#include "MainWidget.h"
#include "Blueprint/UserWidget.h"

APalMainMenuGameMode::APalMainMenuGameMode()
{
	DefaultPawnClass = nullptr;
}

void APalMainMenuGameMode::BeginPlay()
{
	// 1. Check if the GameMode itself is running
	UE_LOG(LogTemp, Warning, TEXT("DEBUG: APalMainMenuGameMode::BeginPlay has STARTED!"));

	Super::BeginPlay();

	// 2. Check the Widget Class
	if (MainWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("DEBUG: MainWidgetClass is VALID. Attempting to CreateWidget..."));

		UMainWidget* MenuWidget = CreateWidget<UMainWidget>(GetWorld(), MainWidgetClass);
		if (MenuWidget)
		{
			MenuWidget->AddToViewport();
			UE_LOG(LogTemp, Warning, TEXT("DEBUG: SUCCESS! MainWidget added to Viewport."));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("DEBUG: FAILED to CreateWidget. Object is Null."));
		}
	}
	else
	{
		// This will print in RED if the class is not assigned in the Editor
		UE_LOG(LogTemp, Error, TEXT("DEBUG: FAILURE! MainWidgetClass is NULL. Assign WBP in Editor."));
	}
}