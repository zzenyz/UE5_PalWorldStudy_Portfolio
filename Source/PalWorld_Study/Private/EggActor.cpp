#include "EggActor.h"
#include "Components/StaticMeshComponent.h"

AEggActor::AEggActor()
{
	PrimaryActorTick.bCanEverTick = false;

	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	RootComponent = DefaultSceneRoot;

	EggMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EggMesh"));
	EggMesh->SetupAttachment(RootComponent);
	EggMesh->SetCollisionProfileName(TEXT("BlockAll"));
}

void AEggActor::BeginPlay()
{
	Super::BeginPlay();
}

void AEggActor::SetEggVisual(UStaticMesh* NewMesh)
{
	if (EggMesh && NewMesh)
	{
		EggMesh->SetStaticMesh(NewMesh);
	}
}