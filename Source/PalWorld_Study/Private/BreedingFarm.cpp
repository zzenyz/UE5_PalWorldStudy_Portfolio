#include "BreedingFarm.h"
#include "EggActor.h"
#include "TimerManager.h"
#include "PalStorageComponent.h"
#include "Blueprint/UserWidget.h"
#include "BreedingWidget.h"
#include "Components/ScrollBox.h"

ABreedingFarm::ABreedingFarm()
{
	PrimaryActorTick.bCanEverTick = false;
	FarmMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FarmMesh"));
	RootComponent = FarmMesh;

	ParentASpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("ParentASpawnPoint"));
	ParentASpawnPoint->SetupAttachment(RootComponent);

	ParentBSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("ParentBSpawnPoint"));
	ParentBSpawnPoint->SetupAttachment(RootComponent);

	EggSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("EggSpawnPoint"));
	EggSpawnPoint->SetupAttachment(RootComponent);

	FarmStorageComponent = CreateDefaultSubobject<UPalStorageComponent>(TEXT("FarmStorageComponent"));

	if (FarmStorageComponent)
	{
		FarmStorageComponent->StorageCapacity = 2;
	}
}

void ABreedingFarm::BeginPlay()
{
	Super::BeginPlay();

	if (FarmStorageComponent)
	{
		FarmStorageComponent->StoredPalsData.SetNum(FarmStorageComponent->StorageCapacity);
		FarmStorageComponent->OnPalStorageUpdated.AddDynamic(this, &ABreedingFarm::UpdateParentSpawns);
	}
}

void ABreedingFarm::UpdateParentSpawns()
{
	if (!FarmStorageComponent) return;
	UWorld* World = GetWorld();
	if (!World) return;

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	auto ManageSpawn = [&](int32 Index, APal*& CurrentSpawned, USceneComponent* SpawnPoint)
		{
			bool bHasData = FarmStorageComponent->StoredPalsData.IsValidIndex(Index);
			FPalData Data = bHasData ? FarmStorageComponent->StoredPalsData[Index] : FPalData();

			if (CurrentSpawned)
			{
				CurrentSpawned->Destroy();
				CurrentSpawned = nullptr;
			}

			if (bHasData && !Data.PalName.IsEmpty())
			{
				if (Data.PalClass)
				{
					CurrentSpawned = World->SpawnActor<APal>(Data.PalClass, SpawnPoint->GetComponentTransform(), Params);
					if (CurrentSpawned)
					{
						CurrentSpawned->PalData = Data;
						CurrentSpawned->OnUpdateColor();
						CurrentSpawned->AIState = 5;
					}
				}
			}
		};

	ManageSpawn(0, SpawnedParentA, ParentASpawnPoint);
	ManageSpawn(1, SpawnedParentB, ParentBSpawnPoint);
}

void ABreedingFarm::StartBreeding()
{
	if (bIsBreeding || bIsEggReady) return;
	if (!FarmStorageComponent || FarmStorageComponent->StoredPalsData.Num() < 2) return;

	FPalData DataA = FarmStorageComponent->StoredPalsData[0];
	FPalData DataB = FarmStorageComponent->StoredPalsData[1];

	if (DataA.PalName.IsEmpty() || DataB.PalName.IsEmpty()) return;

	if (DataA.PalName != DataB.PalName)
	{
		return;
	}

	bIsBreeding = true;
	GetWorldTimerManager().SetTimer(BreedingTimerHandle, this, &ABreedingFarm::OnEggProduced, 20.0f, false);
}

void ABreedingFarm::OnEggProduced()
{
	bIsBreeding = false;
	bIsEggReady = true;

	if (EggClass && FarmStorageComponent && FarmStorageComponent->StoredPalsData.Num() >= 2)
	{
		FPalData DataA = FarmStorageComponent->StoredPalsData[0];
		FPalData DataB = FarmStorageComponent->StoredPalsData[1];

		FPalData ChildData = APal::Breed(DataA, DataB);

		CurrentSpawnedEgg = GetWorld()->SpawnActor<AEggActor>(EggClass, EggSpawnPoint->GetComponentTransform());
		if (CurrentSpawnedEgg)
		{
			CurrentSpawnedEgg->StoredPalData = ChildData;
		}
	}
}

void ABreedingFarm::ClaimEgg(APawn* PlayerPawn)
{
	if (!bIsEggReady || !CurrentSpawnedEgg || !PlayerPawn) return;

	UInventoryComponent* Inventory = PlayerPawn->FindComponentByClass<UInventoryComponent>();

	if (Inventory && EggItemAsset)
	{
		bool bAddSuccess = Inventory->AddItem(EggItemAsset, 1);

		if (bAddSuccess)
		{
			for (int32 i = 0; i < Inventory->GetCapacity(); i++)
			{
				if (Inventory->Slots[i].ItemAsset == EggItemAsset && !Inventory->Slots[i].bHasPalData)
				{
					Inventory->Slots[i].StoredPalData = CurrentSpawnedEgg->StoredPalData;
					Inventory->Slots[i].bHasPalData = true;

					if (Inventory->OnInventoryUpdated.IsBound())
					{
						Inventory->OnInventoryUpdated.Broadcast();
					}
					break;
				}
			}

			CurrentSpawnedEgg->Destroy();
			CurrentSpawnedEgg = nullptr;
			bIsEggReady = false;

			StartBreeding();
		}
	}
}

float ABreedingFarm::GetRemainingTime() const
{
	if (GetWorldTimerManager().IsTimerActive(BreedingTimerHandle))
	{
		return GetWorldTimerManager().GetTimerRemaining(BreedingTimerHandle);
	}
	return 0.0f;
}

void ABreedingFarm::SetupBreedingUI(UUserWidget* InWidget)
{
	if (UBreedingWidget* BreedWidget = Cast<UBreedingWidget>(InWidget))
	{
		BreedWidget->SetTargetFarm(this);
	}
}