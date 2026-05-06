#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PalMainMenuGameMode.generated.h"

// MainWidget 클래스를 쓰겠다고 미리 알려줍니다.
class UMainWidget;

UCLASS()
class PALWORLD_STUDY_API APalMainMenuGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	APalMainMenuGameMode();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<class UMainWidget> MainWidgetClass;
};