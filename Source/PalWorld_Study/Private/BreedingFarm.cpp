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

	// 번식장은 부모 팰 2마리만 관리하므로 용량을 2로 설정합니다.
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
		// [추가된 코드] 배열의 크기를 강제로 StorageCapacity(2)만큼 초기화합니다.
		FarmStorageComponent->StoredPalsData.SetNum(FarmStorageComponent->StorageCapacity);

		// 델리게이트 연결
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

	// 스폰 헬퍼 람다
	auto ManageSpawn = [&](int32 Index, APal*& CurrentSpawned, USceneComponent* SpawnPoint)
		{
			bool bHasData = FarmStorageComponent->StoredPalsData.IsValidIndex(Index);
			FPalData Data = bHasData ? FarmStorageComponent->StoredPalsData[Index] : FPalData();

			// 1. 기존 액터가 있는데 데이터가 없어졌거나 바뀌었다면 삭제
			if (CurrentSpawned)
			{
				CurrentSpawned->Destroy();
				CurrentSpawned = nullptr;
			}

			// 2. 유효한 데이터가 있다면 해당 위치에 스폰
			if (bHasData && !Data.PalName.IsEmpty())
			{
				// [수정된 부분] APal::StaticClass() 대신 Data.PalClass를 사용합니다.
				if (Data.PalClass)
				{
					CurrentSpawned = World->SpawnActor<APal>(Data.PalClass, SpawnPoint->GetComponentTransform(), Params);
					if (CurrentSpawned)
					{
						CurrentSpawned->PalData = Data;
						CurrentSpawned->OnUpdateColor();
						CurrentSpawned->AIState = 5; // 번식장 대기 상태
					}
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("[Farm] PalClass is null in Stored Data!"));
				}
			}
		};

	// 0번 칸은 Parent A 위치에, 1번 칸은 Parent B 위치에 스폰!
	ManageSpawn(0, SpawnedParentA, ParentASpawnPoint);
	ManageSpawn(1, SpawnedParentB, ParentBSpawnPoint);
}

void ABreedingFarm::StartBreeding()
{
	// 1. 상태 및 중복 실행 방지
	if (bIsBreeding || bIsEggReady) return;

	// 2. 스토리지에 부모 2마리 데이터가 다 들어왔는지 직접 확인!
	if (!FarmStorageComponent || FarmStorageComponent->StoredPalsData.Num() < 2) return;

	FPalData DataA = FarmStorageComponent->StoredPalsData[0];
	FPalData DataB = FarmStorageComponent->StoredPalsData[1];

	// 빈 데이터인지 확인 (이름이 None인지)
	if (DataA.PalName.IsEmpty() || DataB.PalName.IsEmpty()) return;

	// 3. 종(이름) 일치 확인
	if (DataA.PalName != DataB.PalName)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Farm] Species mismatch in storage. Cannot breed."));
		return;
	}

	// 4. 모든 조건 클리어 -> 3분(180초) 알 생산 타이머 가동!
	bIsBreeding = true;
	GetWorldTimerManager().SetTimer(BreedingTimerHandle, this, &ABreedingFarm::OnEggProduced, 20.0f, false);

	UE_LOG(LogTemp, Warning, TEXT("[Farm] Factory Started! 3 minutes until next egg."));
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
			UE_LOG(LogTemp, Warning, TEXT("[Farm] Egg Produced successfully from Breed Logic!"));
		}
	}
}

void ABreedingFarm::ClaimEgg(APawn* PlayerPawn)
{
	if (!bIsEggReady || !CurrentSpawnedEgg || !PlayerPawn) return;

	UInventoryComponent* Inventory = PlayerPawn->FindComponentByClass<UInventoryComponent>();

	// 1. 인벤토리와 알 에셋이 모두 유효한지 확인
	if (Inventory && EggItemAsset)
	{
		// 2. [진짜 호출] 인벤토리에 알 아이템 1개 추가 시도!
		bool bAddSuccess = Inventory->AddItem(EggItemAsset, 1);

		if (bAddSuccess)
		{
			// 3. 인벤토리를 뒤져서 방금 들어간 빈 알을 찾고, 팰 유전 정보 주입!
			for (int32 i = 0; i < Inventory->GetCapacity(); i++)
			{
				if (Inventory->Slots[i].ItemAsset == EggItemAsset && !Inventory->Slots[i].bHasPalData)
				{
					Inventory->Slots[i].StoredPalData = CurrentSpawnedEgg->StoredPalData;
					Inventory->Slots[i].bHasPalData = true;

					// UI 업데이트 신호
					if (Inventory->OnInventoryUpdated.IsBound())
					{
						Inventory->OnInventoryUpdated.Broadcast();
					}
					break; // 하나만 찾아서 넣으면 끝
				}
			}

			// 4. 월드의 알 제거 및 초기화
			CurrentSpawnedEgg->Destroy();
			CurrentSpawnedEgg = nullptr;
			bIsEggReady = false;

			// 5. 다음 번식 사이클 시작
			StartBreeding();

			UE_LOG(LogTemp, Warning, TEXT("[Farm] 알 획득 성공! 개체값이 저장된 채로 진짜 인벤토리에 들어갔습니다."));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[Farm] 획득 실패: 인벤토리가 꽉 찼습니다!"));
		}
	}
	else if (!EggItemAsset)
	{
		UE_LOG(LogTemp, Error, TEXT("[Farm] EggItemAsset이 지정되지 않았습니다! 블루프린트 디테일 패널을 확인하세요."));
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
		// 델리게이트와 슬롯 초기화를 이 함수 하나로 끝냅니다.
		BreedWidget->SetTargetFarm(this);
		UE_LOG(LogTemp, Warning, TEXT("[BreedingFarm] UI Setup via SetTargetFarm Complete."));
	}
}