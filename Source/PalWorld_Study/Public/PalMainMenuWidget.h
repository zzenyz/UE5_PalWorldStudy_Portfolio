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

	// 블루프린트 UI에 있는 버튼들과 정확히 이름이 같아야 연결됩니다!
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_NewGame;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_LoadGame;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Exit;

	// 버튼 클릭 시 실행될 함수들
	UFUNCTION()
	void OnNewGameClicked();

	UFUNCTION()
	void OnLoadGameClicked();

	UFUNCTION()
	void OnExitClicked();
};