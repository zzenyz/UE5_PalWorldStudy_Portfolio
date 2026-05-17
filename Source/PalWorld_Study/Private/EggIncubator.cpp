#include "EggIncubator.h"
#include "Pal.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"
#include "PalStorageComponent.h" 

AEggIncubator::AEggIncubator()
{
	PrimaryActorTick.bCanEverTick = false;

	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	RootComponent = DefaultSceneRoot;

	IncubatorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("IncubatorMesh"));
	IncubatorMesh->SetupAttachment(RootComponent);

	EggSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("EggSpawnPoint"));
	EggSpawnPoint->SetupAttachment(IncubatorMesh);

	IncubatorInventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("IncubatorInventory"));
}

void AEggIncubator::BeginPlay()
{
	Super::BeginPlay();

	if (IncubatorInventory)
	{
		IncubatorInventory->Slots.SetNum(1);
		IncubatorInventory->OnInventoryUpdated.AddDynamic(this, &AEggIncubator::OnIncubatorItemChanged);
	}
}

void AEggIncubator::StartHatching(UInventoryComponent* PlayerInventory, int32 EggSlotIndex)
{
	// 방어 코드: 인벤토리 검사 및 중복 실행 방지
	if (!PlayerInventory || !PlayerInventory->Slots.IsValidIndex(EggSlotIndex) || bHasEgg) return;

	FItemSlot& EggSlot = PlayerInventory->Slots[EggSlotIndex];

	if (!EggSlot.bHasPalData) return;

	ContainedPalData = EggSlot.StoredPalData;
	bHasEgg = true;
	bIsHatched = false;

	if (VisualEggClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnedVisualEgg = GetWorld()->SpawnActor<AActor>(VisualEggClass, EggSpawnPoint->GetComponentTransform(), SpawnParams);
	}

	GetWorldTimerManager().SetTimer(HatchTimerHandle, this, &AEggIncubator::OnHatchingComplete, HatchingTime, false);

	// 인벤토리에서 알 제거
	EggSlot.Quantity--;

	if (EggSlot.Quantity <= 0)
	{
		EggSlot.ItemAsset = nullptr;
		EggSlot.Quantity = 0;
		EggSlot.bHasPalData = false;
		EggSlot.StoredPalData = FPalData();
	}

	if (PlayerInventory->OnInventoryUpdated.IsBound())
	{
		PlayerInventory->OnInventoryUpdated.Broadcast();
	}
}

void AEggIncubator::OnHatchingComplete()
{
	bIsHatched = true;
	// TODO: 알 흔들림, 파티클 연출 추가
}

void AEggIncubator::ClaimPal(APawn* PlayerPawn)
{
	if (!bIsHatched || !PlayerPawn) return;

	UPalStorageComponent* StorageComp = PlayerPawn->FindComponentByClass<UPalStorageComponent>();
	UWorld* World = GetWorld();

	if (StorageComp && World)
	{
		if (!ContainedPalData.PalClass) return;

		// 임시 팰 소환 후 스토리지 저장
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		APal* TempPal = World->SpawnActor<APal>(ContainedPalData.PalClass, GetActorTransform(), SpawnParams);

		if (TempPal)
		{
			ContainedPalData.bIsBred = true;
			TempPal->PalData = ContainedPalData;

			bool bSuccess = StorageComp->AddPal(TempPal, nullptr);

			if (bSuccess)
			{
				if (IncubatorInventory && IncubatorInventory->Slots.IsValidIndex(0))
				{
					IncubatorInventory->Slots[0] = FItemSlot();
					IncubatorInventory->OnInventoryUpdated.Broadcast();
				}

				if (SpawnedVisualEgg)
				{
					SpawnedVisualEgg->Destroy();
					SpawnedVisualEgg = nullptr;
				}

				bHasEgg = false;
				bIsHatched = false;
				ContainedPalData = FPalData();
			}
			else
			{
				// 저장소 포화 시 소환된 임시 팰 파괴
				TempPal->Destroy();
			}
		}
	}
}

void AEggIncubator::OnIncubatorItemChanged()
{
	if (!IncubatorInventory) return;

	if (IncubatorInventory->Slots[0].ItemAsset != nullptr && IncubatorInventory->Slots[0].bHasPalData)
	{
		ContainedPalData = IncubatorInventory->Slots[0].StoredPalData;
		bHasEgg = true;
		bIsHatched = false;

		if (VisualEggClass && !SpawnedVisualEgg)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnedVisualEgg = GetWorld()->SpawnActor<AActor>(VisualEggClass, EggSpawnPoint->GetComponentTransform(), SpawnParams);
		}

		GetWorldTimerManager().SetTimer(HatchTimerHandle, this, &AEggIncubator::OnHatchingComplete, HatchingTime, false);
	}
	else
	{
		// 부화 중지 및 초기화
		bHasEgg = false;
		bIsHatched = false;
		GetWorldTimerManager().ClearTimer(HatchTimerHandle);

		if (SpawnedVisualEgg)
		{
			SpawnedVisualEgg->Destroy();
			SpawnedVisualEgg = nullptr;
		}
	}
}