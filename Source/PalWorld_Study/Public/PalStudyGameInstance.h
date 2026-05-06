#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "PalStudyGameInstance.generated.h"

class UDataTable;

UCLASS()
class PALWORLD_STUDY_API UPalStudyGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UPalStudyGameInstance();

	// 세이브 파일 이름 기본값 (나중에 슬롯을 여러 개 만들 때 활용 가능)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SaveSystem")
	FString SaveSlotName = TEXT("PalStudy_SaveSlot_1");

	// 블루프린트나 다른 C++ 클래스에서 호출할 저장 함수
	UFUNCTION(BlueprintCallable, Category = "SaveSystem")
	void SavePlayerData();

	// 블루프린트나 다른 C++ 클래스에서 호출할 불러오기 함수
	UFUNCTION(BlueprintCallable, Category = "SaveSystem")
	void LoadPlayerData();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	UDataTable* ItemDataTable;

	UPROPERTY(BlueprintReadWrite, Category = "SaveSystem")
	bool bShouldLoadData = false;
};