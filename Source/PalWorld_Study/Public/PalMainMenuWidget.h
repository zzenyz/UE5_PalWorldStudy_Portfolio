#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PalMainMenuWidget.generated.h"

class UButton;

UCLASS()
class PALWORLD_STUDY_API UPalMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_NewGame;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_LoadGame;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Exit;

	UFUNCTION()
	void OnNewGameClicked();

	UFUNCTION()
	void OnLoadGameClicked();

	UFUNCTION()
	void OnExitClicked();
};