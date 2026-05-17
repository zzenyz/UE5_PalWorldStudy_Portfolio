#include "PalStorageComponent.h"
#include "Pal.h"
#include "Engine/DataTable.h"

UPalStorageComponent::UPalStorageComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPalStorageComponent::BeginPlay()
{
	Super::BeginPlay();

	StoredPalsData.SetNum(StorageCapacity);
	SpawnedPals.SetNum(StorageCapacity);
}

bool UPalStorageComponent::AddPal(APal* PalToAdd, UPalStorageComponent* OverflowStorage)
{
	if (!PalToAdd) return false;
	APalWorld_StudyCharacter* OwnerCharacter = Cast<APalWorld_StudyCharacter>(GetOwner());
	if (!OwnerCharacter) return false;

	int32 EmptySlotIndex = StoredPalsData.IndexOfByPredicate(
		[](const FPalData& Data) { return Data.PalClass == nullptr; }
	);

	if (EmptySlotIndex == INDEX_NONE)
	{
		if (OverflowStorage != nullptr)
		{
			return OverflowStorage->AddPal(PalToAdd, nullptr);
		}
		return false;
	}

	FPalData NewPalData = PalToAdd->PalData;
	NewPalData.PalClass = PalToAdd->GetClass();
	NewPalData.bIsCaptured = true;
	NewPalData.OwnerCharacter = OwnerCharacter;

	StoredPalsData[EmptySlotIndex] = NewPalData;
	SpawnedPals[EmptySlotIndex] = nullptr;

	PalToAdd->Destroy();

	if (OnPalStorageUpdated.IsBound())
	{
		OnPalStorageUpdated.Broadcast();
	}

	return true;
}

bool UPalStorageComponent::RemovePalByIndex(int32 Index)
{
	if (!StoredPalsData.IsValidIndex(Index))
	{
		return false;
	}

	UnsummonPalByIndex(Index);
	StoredPalsData[Index] = FPalData();

	if (OnPalStorageUpdated.IsBound())
	{
		OnPalStorageUpdated.Broadcast();
	}

	return true;
}

bool UPalStorageComponent::SwapPals(int32 SourceIndex, UPalStorageComponent* DestinationStorage, int32 DestinationIndex)
{
	if (!DestinationStorage ||
		!this->StoredPalsData.IsValidIndex(SourceIndex) ||
		!this->SpawnedPals.IsValidIndex(SourceIndex) ||
		!DestinationStorage->StoredPalsData.IsValidIndex(DestinationIndex) ||
		!DestinationStorage->SpawnedPals.IsValidIndex(DestinationIndex))
	{
		return false;
	}

	this->UnsummonPalByIndex(SourceIndex);
	DestinationStorage->UnsummonPalByIndex(DestinationIndex);

	Swap(this->StoredPalsData[SourceIndex], DestinationStorage->StoredPalsData[DestinationIndex]);
	Swap(this->SpawnedPals[SourceIndex], DestinationStorage->SpawnedPals[DestinationIndex]);

	// Destination 팰 스폰 (Active 상태일 경우)
	AActor* DestOwner = DestinationStorage->GetOwner();
	if (DestOwner && DestinationStorage->bIsActiveInventory && DestinationStorage->StoredPalsData[DestinationIndex].PalClass != nullptr)
	{
		const FTransform OwnerTransform = DestOwner->GetActorTransform();
		const FVector ForwardVector = OwnerTransform.GetRotation().GetForwardVector();
		const float SpawnOffset = -250.0f;
		FVector NewSpawnLocation = OwnerTransform.GetLocation() + (ForwardVector * SpawnOffset);
		NewSpawnLocation.Z += 100.0f;
		const FTransform NewSpawnTransform = FTransform(OwnerTransform.GetRotation(), NewSpawnLocation);

		APal* SpawnedPal = DestinationStorage->SpawnPal(DestinationIndex, NewSpawnTransform, DestOwner);
		if (SpawnedPal) SpawnedPal->AIState = 2;
	}

	// Source 팰 스폰 (Active 상태일 경우)
	AActor* SourceOwner = this->GetOwner();
	if (SourceOwner && this->bIsActiveInventory && this->StoredPalsData[SourceIndex].PalClass != nullptr)
	{
		const FTransform OwnerTransform = SourceOwner->GetActorTransform();
		const FVector ForwardVector = OwnerTransform.GetRotation().GetForwardVector();
		const float SpawnOffset = -250.0f;
		FVector NewSpawnLocation = OwnerTransform.GetLocation() + (ForwardVector * SpawnOffset);
		NewSpawnLocation.Z += 100.0f;
		const FTransform NewSpawnTransform = FTransform(OwnerTransform.GetRotation(), NewSpawnLocation);

		APal* SpawnedPal = this->SpawnPal(SourceIndex, NewSpawnTransform, SourceOwner);
		if (SpawnedPal) SpawnedPal->AIState = 2;
	}


	if (this->OnPalStorageUpdated.IsBound())
	{
		this->OnPalStorageUpdated.Broadcast();
	}

	if (DestinationStorage != this && DestinationStorage->OnPalStorageUpdated.IsBound())
	{
		DestinationStorage->OnPalStorageUpdated.Broadcast();
	}

	return true;
}

APal* UPalStorageComponent::SpawnPal(int32 PalIndex, const FTransform& SpawnTransform, AActor* Spawner)
{
	if (!SpawnedPals.IsValidIndex(PalIndex) || !StoredPalsData.IsValidIndex(PalIndex) || !Spawner) return nullptr;

	UWorld* World = GetWorld();
	if (!World) return nullptr;

	const FPalData& PalDataToSpawn = StoredPalsData[PalIndex];
	if (PalDataToSpawn.PalClass == nullptr)
	{
		return nullptr;
	}

	if (SpawnedPals[PalIndex].IsValid())
	{
		return SpawnedPals[PalIndex].Get();
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	APal* SpawnedPal = World->SpawnActor<APal>(PalDataToSpawn.PalClass, SpawnTransform, SpawnParams);

	if (SpawnedPal)
	{
		SpawnedPal->PalData = PalDataToSpawn;
		SpawnedPal->SpawnerActor = Spawner;
		SpawnedPal->RefreshPalState();

		SpawnedPals[PalIndex] = SpawnedPal;
	}

	return SpawnedPal;
}

bool UPalStorageComponent::UnsummonPalByIndex(int32 PalIndex)
{
	if (!SpawnedPals.IsValidIndex(PalIndex) || !StoredPalsData.IsValidIndex(PalIndex) || !SpawnedPals[PalIndex].IsValid())
	{
		return false;
	}

	APal* PalToUnsummon = SpawnedPals[PalIndex].Get();

	if (PalToUnsummon)
	{
		StoredPalsData[PalIndex] = PalToUnsummon->PalData;
		PalToUnsummon->Destroy();
	}

	SpawnedPals[PalIndex] = nullptr;

	return true;
}

bool UPalStorageComponent::UnsummonAllPals()
{
	for (int32 i = 0; i < SpawnedPals.Num(); ++i)
	{
		UnsummonPalByIndex(i);
	}
	return true;
}

#if WITH_EDITOR
void UPalStorageComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(UPalStorageComponent, StorageCapacity))
	{
		StoredPalsData.SetNum(StorageCapacity);
		SpawnedPals.SetNum(StorageCapacity);
	}

	if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(UPalStorageComponent, StoredPalsData))
	{
		if (!PalDataTable) return;

		for (FPalData& PalData : StoredPalsData)
		{
			if (PalData.PalClass != nullptr && (PalData.PalName.IsEmpty() || PalData.PalName == "None"))
			{
				TArray<FName> RowNames = PalDataTable->GetRowNames();
				for (const FName& RowName : RowNames)
				{
					FPalData* FoundRowData = PalDataTable->FindRow<FPalData>(RowName, TEXT(""));
					if (FoundRowData && FoundRowData->PalClass == PalData.PalClass)
					{
						PalData.PalName = FoundRowData->PalName;
						PalData.PalIcon = FoundRowData->PalIcon;
						break;
					}
				}
			}
		}
	}
}
#endif