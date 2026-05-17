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
	Super::BeginPlay();

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC && MainWidgetClass)
	{
		UUserWidget* MainMenu = CreateWidget<UUserWidget>(PC, MainWidgetClass);
		if (MainMenu)
		{
			MainMenu->AddToViewport(100);

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
		PalInventory->UnsummonPalByIndex(IndexToHandle);
	}
	else
	{
		APalWorld_StudyCharacter* MyPlayer = Cast<APalWorld_StudyCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
		if (!MyPlayer) return;

		FTransform SpawnTransform = MyPlayer->GetActorTransform();
		const FVector ForwardVector = MyPlayer->GetActorForwardVector();
		FVector NewSpawnLocation = MyPlayer->GetActorLocation() + (ForwardVector * 250.0f);
		NewSpawnLocation.Z += 100.0f;
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