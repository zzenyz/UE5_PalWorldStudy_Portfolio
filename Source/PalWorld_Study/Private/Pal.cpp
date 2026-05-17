#include "Pal.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Algo/RandomShuffle.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Controller.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "AIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PalWorld_StudyCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

APal::APal()
{
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);

	PrimaryActorTick.bCanEverTick = true;

	BaseXP = 100.0f;
	Exponent = 2.1f;

	CarriedItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CarriedItemMesh"));
	CarriedItemMesh->SetupAttachment(RootComponent);
	CarriedItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CarriedItemMesh->SetVisibility(false);
}

void APal::BeginPlay()
{
	Super::BeginPlay();

	if (PalDataTable && !PalSpeciesID.IsNone())
	{
		static const FString ContextString(TEXT("Pal Data Context"));
		FPalData* RowData = PalDataTable->FindRow<FPalData>(PalSpeciesID, ContextString, true);

		if (RowData)
		{
			PalData.PalName = RowData->PalName;
			PalData.PalClass = RowData->PalClass;
			PalData.PalIcon = RowData->PalIcon;
			PalData.BaseMaxHP = RowData->BaseMaxHP;
			PalData.BaseAttack = RowData->BaseAttack;
			PalData.BaseDefense = RowData->BaseDefense;
			PalData.AttackMontage = RowData->AttackMontage;
		}
	}

	if (!PalData.bIsCaptured)
	{
		// 랜덤 베이스 색상
		FLinearColor HSV;
		HSV.R = FMath::FRandRange(0.0f, 360.0f);
		HSV.G = 1.0f;
		HSV.B = 1.0f;
		PalData.BodyColor = FLinearColor::MakeFromHSV8(HSV.R, HSV.G * 255, HSV.B * 255);

		PalData.Level = FMath::RandRange(1, 5);

		// 랜덤 개체값(IV)
		PalData.HP_IV = FMath::FRandRange(0.0f, 100.0f);
		PalData.Attack_IV = FMath::FRandRange(0.0f, 100.0f);
		PalData.Defense_IV = FMath::FRandRange(0.0f, 100.0f);

		// 특성 부여
		PalData.Traits.Empty();
		const TMap<EPalTraitType, FPalTrait>& TraitMapRef = GetTraitMap();
		if (TraitMapRef.Num() > 0)
		{
			TArray<EPalTraitType> AllTraitTypes;
			TraitMapRef.GenerateKeyArray(AllTraitTypes);
			Algo::RandomShuffle(AllTraitTypes);

			int32 NumTraitsToAssign = FMath::RandRange(0, 4);
			for (int32 i = 0; i < NumTraitsToAssign && i < AllTraitTypes.Num(); ++i)
			{
				PalData.Traits.Add(AllTraitTypes[i]);
			}
		}
		OnUpdateColor();
		RecalculateStats();
		PalData.CurrentHP = FinalMaxHP;
	}

	if (PalData.bIsBred && PalData.Level < 5)
	{
		SetActorScale3D(FVector(0.7f, 0.7f, 0.7f));
	}
}

void APal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APal::RefreshPalState()
{
	if (PalData.bIsBred && PalData.Level < 5)
	{
		SetActorScale3D(FVector(0.7f, 0.7f, 0.7f));
	}
	else
	{
		SetActorScale3D(FVector(1.0f, 1.0f, 1.0f));
	}

	OnUpdateColor();
	RecalculateStats();
}

const TMap<EPalTraitType, FPalTrait>& APal::GetTraitMap()
{
	static TMap<EPalTraitType, FPalTrait> TraitMap = {
		{ EPalTraitType::Brave, FPalTrait(1.1f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, false, false, false, 1.0f) },
		{ EPalTraitType::Ferocious, FPalTrait(1.2f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, false, false, false, 2.0f) },
		{ EPalTraitType::HardSkin, FPalTrait(1.0f, 1.1f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, false, false, false, 1.0f) },
		{ EPalTraitType::ToughBody, FPalTrait(1.0f, 1.2f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, false, false, false, 2.0f) },
		{ EPalTraitType::Legendary, FPalTrait(1.2f, 1.2f, 1.15f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, false, false, false, 6.0f) },
	};
	return TraitMap;
}

FPalData APal::Breed(const FPalData& ParentA, const FPalData& ParentB)
{
	FPalData ChildData;

	// 기본 종족 정보 유전
	ChildData.OwnerCharacter = ParentA.OwnerCharacter;
	ChildData.bIsCaptured = true;
	ChildData.bIsBred = true;
	ChildData.Level = 1;
	ChildData.PalClass = ParentA.PalClass;
	ChildData.PalName = ParentA.PalName;
	ChildData.PalIcon = ParentA.PalIcon;
	ChildData.BaseMaxHP = ParentA.BaseMaxHP;
	ChildData.BaseAttack = ParentA.BaseAttack;
	ChildData.BaseDefense = ParentA.BaseDefense;

	// 개체값 (IV) 확률 유전
	float HPRoll = FMath::FRand();
	if (HPRoll < 0.3f) ChildData.HP_IV = ParentA.HP_IV;
	else if (HPRoll < 0.6f) ChildData.HP_IV = ParentB.HP_IV;
	else ChildData.HP_IV = FMath::FRandRange(0.0f, 100.0f);

	float AttackRoll = FMath::FRand();
	if (AttackRoll < 0.3f) ChildData.Attack_IV = ParentA.Attack_IV;
	else if (AttackRoll < 0.6f) ChildData.Attack_IV = ParentB.Attack_IV;
	else ChildData.Attack_IV = FMath::FRandRange(0.0f, 100.0f);

	float DefenseRoll = FMath::FRand();
	if (DefenseRoll < 0.3f) ChildData.Defense_IV = ParentA.Defense_IV;
	else if (DefenseRoll < 0.6f) ChildData.Defense_IV = ParentB.Defense_IV;
	else ChildData.Defense_IV = FMath::FRandRange(0.0f, 100.0f);

	// 특성 (Traits) 유전 및 돌연변이
	ChildData.Traits.Empty();
	const float InheritanceChance = 0.6f;
	const float MutationChance = 0.2f;

	if (ParentA.Traits.Num() > 0 && FMath::FRand() < InheritanceChance)
		ChildData.Traits.AddUnique(ParentA.Traits[FMath::RandRange(0, ParentA.Traits.Num() - 1)]);

	if (ParentB.Traits.Num() > 0 && FMath::FRand() < InheritanceChance)
		ChildData.Traits.AddUnique(ParentB.Traits[FMath::RandRange(0, ParentB.Traits.Num() - 1)]);

	if (FMath::FRand() < MutationChance)
	{
		TArray<EPalTraitType> AllTraitTypes;
		GetTraitMap().GenerateKeyArray(AllTraitTypes);
		if (AllTraitTypes.Num() > 0)
		{
			ChildData.Traits.AddUnique(AllTraitTypes[FMath::RandRange(0, AllTraitTypes.Num() - 1)]);
		}
	}

	// 최종 등급 (StrengthTier) 산출
	float AverageIV_Score = (ChildData.HP_IV + ChildData.Attack_IV + ChildData.Defense_IV) / 3.0f;
	float TotalTrait_Score = 0.0f;

	const TMap<EPalTraitType, FPalTrait>& TraitMap = GetTraitMap();
	for (const EPalTraitType& TraitType : ChildData.Traits)
	{
		if (const FPalTrait* TraitData = TraitMap.Find(TraitType))
		{
			TotalTrait_Score += TraitData->TraitScore;
		}
	}

	float FinalScore = AverageIV_Score + TotalTrait_Score;
	const float MinFinalScore = -10.0f;
	const float MaxFinalScore = 120.0f;

	float StrengthTier = FMath::GetMappedRangeValueClamped(
		FVector2D(MinFinalScore, MaxFinalScore),
		FVector2D(0.0f, 1.0f),
		FinalScore
	);

	// 색상 혼합 및 등급별 발광 적용
	float MixRatio = FMath::FRand();
	FLinearColor MixedRGB = FMath::Lerp(ParentA.BodyColor, ParentB.BodyColor, MixRatio);
	FLinearColor MixedHSV = MixedRGB.LinearRGBToHSV();

	MixedHSV.G = 1.0f;
	MixedHSV.B = 1.0f;

	FLinearColor SaturatedColor = MixedHSV.HSVToLinearRGB();
	float ColorBlendAlpha = FMath::Pow(StrengthTier, 3.0f);
	ChildData.BodyColor = FMath::Lerp(SaturatedColor, FLinearColor::White, ColorBlendAlpha);

	return ChildData;
}

float APal::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	AController* MyController = GetController();

	if (PalData.CurrentHP <= 0.f) return 0.f;

	if (ActualDamage > 0.f)
	{
		PalData.CurrentHP -= ActualDamage;

		if (PalData.CurrentHP <= 0)
		{
			USkeletalMeshComponent* MeshComp = GetMesh();
			if (MeshComp)
			{
				MeshComp->SetCollisionProfileName(TEXT("Ragdoll"));
				MeshComp->SetSimulatePhysics(true);
				MeshComp->WakeAllRigidBodies();
				MeshComp->bBlendPhysics = true;
			}
			if (MyController)
			{
				MyController->UnPossess();
			}
		}
	}
	return ActualDamage;
}

float APal::GetXPToNextLevel() const
{
	return BaseXP * FMath::Pow(PalData.Level, Exponent);
}

void APal::GainExperience(float Amount)
{
	PalData.CurrentXP += Amount;
	CheckLevelUp();
}

void APal::CheckLevelUp()
{
	while (PalData.CurrentHP > 0 && PalData.CurrentXP >= GetXPToNextLevel())
	{
		PalData.CurrentXP -= GetXPToNextLevel();
		LevelUp();
	}
}

void APal::LevelUp()
{
	PalData.Level++;

	if (PalData.bIsBred && PalData.Level >= 5)
	{
		SetActorScale3D(FVector(1.0f, 1.0f, 1.0f));
	}

	PalData.BaseMaxHP += 1.0f;
	PalData.BaseAttack += 2.0f;
	PalData.BaseDefense += 1.0f;

	RecalculateStats();
	PalData.CurrentHP = FinalMaxHP;
}

void APal::HandleMasterCombat(AActor* TargetEnemy)
{
	if (!TargetEnemy || PalData.CurrentHP <= 0.f) return;

	AAIController* AICon = Cast<AAIController>(GetController());
	if (!AICon) return;

	UBlackboardComponent* BlackboardComp = AICon->GetBlackboardComponent();
	if (!BlackboardComp) return;

	BlackboardComp->SetValueAsObject(TEXT("BattleActor"), TargetEnemy);
	AIState = 3;

	// 디버그 메시지 유지
	if (GEngine)
	{
		FString DebugMsg = FString::Printf(TEXT("%s -> %s Attack!"), *GetName(), *TargetEnemy->GetName());
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, DebugMsg);
	}
}

void APal::SetMaster(AActor* NewMaster)
{
	PalData.OwnerCharacter = Cast<ACharacter>(NewMaster);

	if (APalWorld_StudyCharacter* PlayerMaster = Cast<APalWorld_StudyCharacter>(NewMaster))
	{
		PlayerMaster->OnCombatEngaged.AddDynamic(this, &APal::HandleMasterCombat);
	}
}

void APal::RecalculateStats()
{
	float AttackScale_Total = 1.0f;
	float DefenseScale_Total = 1.0f;

	const TMap<EPalTraitType, FPalTrait>& TraitMap = GetTraitMap();

	for (const EPalTraitType& TraitType : PalData.Traits)
	{
		if (const FPalTrait* TraitData = TraitMap.Find(TraitType))
		{
			AttackScale_Total *= TraitData->AttackScale;
			DefenseScale_Total *= TraitData->DefenseScale;
		}
	}

	FinalMaxHP = (PalData.BaseMaxHP + PalData.HP_IV);
	FinalAttack = (PalData.BaseAttack + PalData.Attack_IV) * AttackScale_Total;
	FinalDefense = (PalData.BaseDefense + PalData.Defense_IV) * DefenseScale_Total;

	PalData.CurrentHP = FMath::Min(PalData.CurrentHP, FinalMaxHP);
}