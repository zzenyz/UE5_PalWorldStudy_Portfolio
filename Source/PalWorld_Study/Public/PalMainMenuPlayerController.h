#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PalMainMenuPlayerController.generated.h"

UCLASS()
class PALWORLD_STUDY_API APalMainMenuPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
};