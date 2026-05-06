#include "EggActor.h"
#include "Components/StaticMeshComponent.h"

AEggActor::AEggActor()
{
	// 알 액터는 매 프레임 계산(Tick)이 필요 없으므로 꺼두는 것이 효율적입니다.
	PrimaryActorTick.bCanEverTick = false;

	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	RootComponent = DefaultSceneRoot;

	EggMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EggMesh"));
	EggMesh->SetupAttachment(RootComponent);

	// 알은 바닥에 놓여 있어야 하므로 물리 설정을 켜거나, 팜의 EggSpawnPoint에 붙게 됩니다.
	EggMesh->SetCollisionProfileName(TEXT("BlockAll"));
}

void AEggActor::BeginPlay()
{
	Super::BeginPlay();

	// Self-Healing: 만약 데이터가 비어있다면 기본값으로 초기화하는 로직을 여기에 넣을 수 있습니다.
}

void AEggActor::SetEggVisual(UStaticMesh* NewMesh)
{
	if (EggMesh && NewMesh)
	{
		EggMesh->SetStaticMesh(NewMesh);
	}
}