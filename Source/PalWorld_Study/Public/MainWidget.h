#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "MainWidget.generated.h"

UCLASS()
class PALWORLD_STUDY_API UMainWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_NewGame;

	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_LoadGame;

	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnNewGameClicked();

	UFUNCTION()
	void OnLoadGameClicked();
};