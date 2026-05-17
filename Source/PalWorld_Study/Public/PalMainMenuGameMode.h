#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PalMainMenuGameMode.generated.h"

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