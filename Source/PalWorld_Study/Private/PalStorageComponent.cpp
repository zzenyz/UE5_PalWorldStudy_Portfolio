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

	// 배열 크기 초기화
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

	// ✅ [Add] 갱신 알림
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

	// ✅ [Remove] 데이터가 지워졌으니 갱신 알림! (이게 빠져 있었음)
	if (OnPalStorageUpdated.IsBound())
	{
		OnPalStorageUpdated.Broadcast();
	}

	return true;
}

bool UPalStorageComponent::SwapPals(int32 SourceIndex, UPalStorageComponent* DestinationStorage, int32 DestinationIndex)
{
	// 1. 유효성 검사
	if (!DestinationStorage ||
		!this->StoredPalsData.IsValidIndex(SourceIndex) ||
		!this->SpawnedPals.IsValidIndex(SourceIndex) ||
		!DestinationStorage->StoredPalsData.IsValidIndex(DestinationIndex) ||
		!DestinationStorage->SpawnedPals.IsValidIndex(DestinationIndex))
	{
		return false;
	}

	// 2. 소환 해제
	this->UnsummonPalByIndex(SourceIndex);
	DestinationStorage->UnsummonPalByIndex(DestinationIndex);

	// 3. 교환
	Swap(this->StoredPalsData[SourceIndex], DestinationStorage->StoredPalsData[DestinationIndex]);
	Swap(this->SpawnedPals[SourceIndex], DestinationStorage->SpawnedPals[DestinationIndex]);

	// 4. 스폰 로직 (Active 상태일 경우)
	// --- Destination 쪽 스폰 ---
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

	// --- Source(내꺼) 쪽 스폰 ---
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
		if (!PalDataTable)
		{
			return;
		}
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