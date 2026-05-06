#include "EggIncubator.h"
#include "Pal.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"
#include "PalStorageComponent.h" // 팰 저장소 컴포넌트 헤더

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
	// 1. 방어 코드: 인벤토리가 없거나, 슬롯 번호가 잘못되었거나, 이미 알이 있으면 취소
	if (!PlayerInventory || !PlayerInventory->Slots.IsValidIndex(EggSlotIndex) || bHasEgg) return;

	// 인벤토리의 해당 슬롯 참조
	FItemSlot& EggSlot = PlayerInventory->Slots[EggSlotIndex];

	// 알 데이터가 없는 일반 아이템이면 취소
	if (!EggSlot.bHasPalData) return;

	// 2. 데이터 저장 및 상태 변경
	ContainedPalData = EggSlot.StoredPalData;
	bHasEgg = true;
	bIsHatched = false;

	// 3. 비주얼 알 스폰
	if (VisualEggClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnedVisualEgg = GetWorld()->SpawnActor<AActor>(VisualEggClass, EggSpawnPoint->GetComponentTransform(), SpawnParams);
	}

	// 4. 부화 타이머 시작
	GetWorldTimerManager().SetTimer(HatchTimerHandle, this, &AEggIncubator::OnHatchingComplete, HatchingTime, false);

	// 🔥 5. [핵심] 인벤토리에서 알 제거 로직
	EggSlot.Quantity--; // 알 개수 1개 감소

	if (EggSlot.Quantity <= 0)
	{
		// 슬롯 완전 초기화 (빈 칸으로 만들기)
		EggSlot.ItemAsset = nullptr;
		EggSlot.Quantity = 0;
		EggSlot.bHasPalData = false;
		EggSlot.StoredPalData = FPalData();
	}

	// UI 갱신 방송 (인벤토리 화면 새로고침)
	if (PlayerInventory->OnInventoryUpdated.IsBound())
	{
		PlayerInventory->OnInventoryUpdated.Broadcast();
	}

	UE_LOG(LogTemp, Warning, TEXT("[Incubator] Success."));
}

void AEggIncubator::OnHatchingComplete()
{
	bIsHatched = true;
	UE_LOG(LogTemp, Warning, TEXT("complete."));

	// 여기서 알이 흔들리거나 파티클이 터지는 연출을 넣으면 좋아!
}

void AEggIncubator::ClaimPal(APawn* PlayerPawn)
{
	if (!bIsHatched || !PlayerPawn) return;

	UPalStorageComponent* StorageComp = PlayerPawn->FindComponentByClass<UPalStorageComponent>();
	UWorld* World = GetWorld();

	if (StorageComp && World)
	{
		// [추가] 방어 코드: 알 데이터에 팰 클래스가 정상적으로 존재하는지 확인
		if (!ContainedPalData.PalClass)
		{
			UE_LOG(LogTemp, Error, TEXT("[Incubator] Failed: PalClass is null in Egg Data!"));
			return;
		}

		// 1. 임시로 팰 소환 (기존 AddPal에 넣기 위한 액터화)
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

				// 부화기 내부 상태 초기화
				bHasEgg = false;
				bIsHatched = false;
				ContainedPalData = FPalData();
			}
			else
			{
				// 🔥 [핵심 추가] 저장소가 꽉 차서 실패했을 때, 임시로 소환한 팰을 다시 파괴합니다!
				TempPal->Destroy();
			}
		}
	}
}

void AEggIncubator::OnIncubatorItemChanged()
{
	if (!IncubatorInventory) return;

	// 0번 슬롯(유일한 칸)에 아이템이 있고, 그게 팰 데이터가 있는 알이라면?
	if (IncubatorInventory->Slots[0].ItemAsset != nullptr && IncubatorInventory->Slots[0].bHasPalData)
	{
		// 알이 들어왔다! 부화 세팅 시작
		ContainedPalData = IncubatorInventory->Slots[0].StoredPalData;
		bHasEgg = true;
		bIsHatched = false;

		// 알 그래픽 띄우기
		if (VisualEggClass && !SpawnedVisualEgg)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnedVisualEgg = GetWorld()->SpawnActor<AActor>(VisualEggClass, EggSpawnPoint->GetComponentTransform(), SpawnParams);
		}

		// 타이머 시작!
		GetWorldTimerManager().SetTimer(HatchTimerHandle, this, &AEggIncubator::OnHatchingComplete, HatchingTime, false);
		UE_LOG(LogTemp, Warning, TEXT("[Incubator] 알 감지! 부화를 시작합니다."));
	}
	else
	{
		// 알이 없거나(뺐거나), 이상한 아이템이 들어왔다면 부화 중지 및 초기화
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