// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerSystem.h"
#include "InventoryComponent.h"
#include "PalStorageComponent.h"
#include "Pal.h" 
#include "PalWorld_StudyCharacter.h"
#include "Kismet/GameplayStatics.h" 

APlayerSystem::APlayerSystem()
{
	Inventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));
	PalInventory = CreateDefaultSubobject<UPalStorageComponent>(TEXT("PalInventory"));
}

void APlayerSystem::BeginPlay()
{
	// 부모의 BeginPlay(입력 설정 등)가 먼저 실행되게 둡니다.
	Super::BeginPlay();

	// 단일 맵 방식: 시작하자마자 메뉴 위젯 생성
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC && MainWidgetClass)
	{
		UUserWidget* MainMenu = CreateWidget<UUserWidget>(PC, MainWidgetClass);
		if (MainMenu)
		{
			MainMenu->AddToViewport(100); // 최상단에 띄움

			PC->bShowMouseCursor = true;
			FInputModeUIOnly InputMode;
			InputMode.SetWidgetToFocus(MainMenu->TakeWidget());
			PC->SetInputMode(InputMode);
		}
	}
}

void APlayerSystem::HandleSummonAction(int32 IndexToHandle)
{
	if (!PalInventory) return;

	if (!PalInventory->StoredPalsData.IsValidIndex(IndexToHandle) || !PalInventory->SpawnedPals.IsValidIndex(IndexToHandle))
	{
		return;
	}

	if (PalInventory->SpawnedPals[IndexToHandle].IsValid())
	{
		UE_LOG(LogTemp, Log, TEXT("HandleSummonAction: 인덱스 %d의 팰을 소환 해제합니다."), IndexToHandle);
		PalInventory->UnsummonPalByIndex(IndexToHandle);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("HandleSummonAction: 인덱스 %d의 팰을 소환합니다."), IndexToHandle);

		// ★ [수정 1] 이제 플레이어를 먼저 확실하게 찾습니다!
		APalWorld_StudyCharacter* MyPlayer = Cast<APalWorld_StudyCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
		if (!MyPlayer) return;

		// ★ [수정 2] 시스템 액터 위치가 아닌, '플레이어 캐릭터'의 위치와 방향을 기준으로 소환합니다!
		FTransform SpawnTransform = MyPlayer->GetActorTransform();
		const FVector ForwardVector = MyPlayer->GetActorForwardVector();
		FVector NewSpawnLocation = MyPlayer->GetActorLocation() + (ForwardVector * 250.0f); // 플레이어 앞 2.5미터
		NewSpawnLocation.Z += 100.0f; // 땅에 안 끼게 살짝 위로
		SpawnTransform.SetLocation(NewSpawnLocation);

		APal* SpawnedPal = PalInventory->SpawnPal(IndexToHandle, SpawnTransform, MyPlayer);

		if (SpawnedPal)
		{
			SpawnedPal->AIState = 1;
			SpawnedPal->SetMaster(MyPlayer);
			SpawnedPal->RefreshPalState();
		}
	}
}