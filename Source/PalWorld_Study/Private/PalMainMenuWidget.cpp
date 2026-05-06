#include "PalMainMenuWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PalStudyGameInstance.h"

void UPalMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 버튼에 클릭 이벤트 연결
	if (Btn_NewGame) Btn_NewGame->OnClicked.AddDynamic(this, &UPalMainMenuWidget::OnNewGameClicked);
	if (Btn_LoadGame) Btn_LoadGame->OnClicked.AddDynamic(this, &UPalMainMenuWidget::OnLoadGameClicked);
	if (Btn_Exit) Btn_Exit->OnClicked.AddDynamic(this, &UPalMainMenuWidget::OnExitClicked);
}

void UPalMainMenuWidget::OnNewGameClicked()
{
	if (UPalStudyGameInstance* GI = Cast<UPalStudyGameInstance>(UGameplayStatics::GetGameInstance(this)))
	{
		GI->bShouldLoadData = false; // 새 게임이므로 로드 안 함
	}

	// "GameMap" 부분을 실제 플레이할 게임 레벨 이름으로 꼭 바꿔주세요!
	UGameplayStatics::OpenLevel(this, FName("GameMap"));
}

void UPalMainMenuWidget::OnLoadGameClicked()
{
	if (UPalStudyGameInstance* GI = Cast<UPalStudyGameInstance>(UGameplayStatics::GetGameInstance(this)))
	{
		// 세이브 파일이 실제로 존재하는지 확인 후 이동
		if (UGameplayStatics::DoesSaveGameExist(GI->SaveSlotName, 0))
		{
			GI->bShouldLoadData = true; // 로드 플래그 켜기
			UGameplayStatics::OpenLevel(this, FName("GameMap"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("세이브 파일이 없습니다!"));
			// 여기에 나중에 UI로 "세이브 파일이 없습니다" 경고창을 띄우면 좋습니다.
		}
	}
}

void UPalMainMenuWidget::OnExitClicked()
{
	// 게임 종료
	UKismetSystemLibrary::QuitGame(this, nullptr, EQuitPreference::Quit, false);
}