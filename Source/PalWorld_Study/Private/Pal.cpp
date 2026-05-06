// Fill out your copyright notice in the Description page of Project Settings.

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
	// 이동 회전 관련
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);

	PrimaryActorTick.bCanEverTick = true;

	// [유기적 수정]
	// PalName, Level, HP 등의 기본값 설정은 FPalData의 생성자에서 처리되므로 삭제합니다.

	// 이 변수들은 APal 고유의 변수이므로 그대로 둡니다.
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

		// 구조체 이름으로 FPalData를 사용합니다!
		FPalData* RowData = PalDataTable->FindRow<FPalData>(PalSpeciesID, ContextString, true);

		if (RowData)
		{
			// 데이터 테이블에 적힌 기본 정보를 현재 팰의 PalData에 덮어씌웁니다.
			PalData.PalName = RowData->PalName;
			PalData.PalClass = RowData->PalClass;
			PalData.PalIcon = RowData->PalIcon;
			PalData.BaseMaxHP = RowData->BaseMaxHP;
			PalData.BaseAttack = RowData->BaseAttack;
			PalData.BaseDefense = RowData->BaseDefense;
			PalData.AttackMontage = RowData->AttackMontage;
		}
	}

	// [유기적 수정] 모든 변수 접근을 'PalData.'를 통해 하도록 변경합니다.
	if (!PalData.bIsCaptured)
	{
		// 1. 랜덤 색상 생성
		FLinearColor HSV;
		HSV.R = FMath::FRandRange(0.0f, 360.0f);
		HSV.G = 1.0f;
		HSV.B = 1.0f;
		PalData.BodyColor = FLinearColor::MakeFromHSV8(HSV.R, HSV.G * 255, HSV.B * 255);

		// 2. 레벨 랜덤 설정 (1 ~ 5)
		PalData.Level = FMath::RandRange(1, 5);

		// 3. 개체값(IV) 랜덤 설정 (0 ~ 100)
		PalData.HP_IV = FMath::FRandRange(0.0f, 100.0f);
		PalData.Attack_IV = FMath::FRandRange(0.0f, 100.0f);
		PalData.Defense_IV = FMath::FRandRange(0.0f, 100.0f);

		// 4. 특성 랜덤 부여 (0 ~ 4개)
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
		OnUpdateColor(); // 블루프린트 이벤트 호출
		RecalculateStats(); // 최종 스탯 계산
		PalData.CurrentHP = FinalMaxHP; // 현재 HP를 최대치로 설정
	}

	if (PalData.bIsBred && PalData.Level < 5)
	{
		SetActorScale3D(FVector(0.7f, 0.7f, 0.7f));
	}
}

// Called every frame
void APal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APal::RefreshPalState()
{
	// 1. 크기(Scale) 갱신: 교배로 태어났고 레벨이 5 미만이면 0.7배율, 아니면 1.0배율
	if (PalData.bIsBred && PalData.Level < 5)
	{
		SetActorScale3D(FVector(0.7f, 0.7f, 0.7f));
	}
	else
	{
		SetActorScale3D(FVector(1.0f, 1.0f, 1.0f));
	}

	// 2. 색상 머티리얼 갱신 (기존에 만들어두신 블루프린트 이벤트 호출)
	OnUpdateColor();

	// 3. 스탯 갱신
	RecalculateStats();
}

const TMap<EPalTraitType, FPalTrait>& APal::GetTraitMap()
{
	// 이 함수는 데이터를 직접 수정하지 않으므로 변경할 필요가 없습니다.
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

	// 1. 기본 정보 유전
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

	// 기본 종족 스탯 고정
	ChildData.BaseMaxHP = ParentA.BaseMaxHP;
	ChildData.BaseAttack = ParentA.BaseAttack;
	ChildData.BaseDefense = ParentA.BaseDefense;

	// ==========================================
	// ★ 2. 개체값 (IV) 확률 유전 복원
	// (30% 확률로 A, 30% 확률로 B, 40% 확률로 돌연변이)
	// ==========================================
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

	// ==========================================
	// ★ 3. 특성 (Traits) 유전 및 돌연변이 복원
	// ==========================================
	ChildData.Traits.Empty();
	const float InheritanceChance = 0.6f;
	const float MutationChance = 0.2f;

	// 부모 A 특성 유전
	if (ParentA.Traits.Num() > 0 && FMath::FRand() < InheritanceChance)
		ChildData.Traits.AddUnique(ParentA.Traits[FMath::RandRange(0, ParentA.Traits.Num() - 1)]);

	// 부모 B 특성 유전
	if (ParentB.Traits.Num() > 0 && FMath::FRand() < InheritanceChance)
		ChildData.Traits.AddUnique(ParentB.Traits[FMath::RandRange(0, ParentB.Traits.Num() - 1)]);

	// 돌연변이 특성 추가
	if (FMath::FRand() < MutationChance)
	{
		TArray<EPalTraitType> AllTraitTypes;
		GetTraitMap().GenerateKeyArray(AllTraitTypes);
		if (AllTraitTypes.Num() > 0)
		{
			ChildData.Traits.AddUnique(AllTraitTypes[FMath::RandRange(0, AllTraitTypes.Num() - 1)]);
		}
	}

	// ==========================================
	// ★ 4. 최종 등급 (StrengthTier) 산출 로직 복원
	// ==========================================
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

	// ==========================================
// ★ 5. 색상 혼합 로직
// ==========================================

// 1. 부모의 RGB 색상을 먼저 섞습니다. (MixRatio에 따라 선형 보간)
float MixRatio = FMath::FRand();
FLinearColor MixedRGB = FMath::Lerp(ParentA.BodyColor, ParentB.BodyColor, MixRatio);

// 2. 섞인 RGB 값을 HSV로 변환합니다.
FLinearColor MixedHSV = MixedRGB.LinearRGBToHSV();

// 3. 채도(Saturation)와 명도(Value)를 최대치로 올립니다.
// (MixedHSV.R 인 색상값은 RGB가 섞이면서 자연스럽게 나온 값을 그대로 유지합니다)
MixedHSV.G = 1.0f; // 채도 최대치
MixedHSV.B = 1.0f; // 명도 최대치 (어두워지는 것 방지)

// 4. 다시 RGB로 변환합니다. (문제를 일으켰던 MakeFromHSV8 대신 HSVToLinearRGB 사용)
FLinearColor SaturatedColor = MixedHSV.HSVToLinearRGB();

// 5. 희귀도(StrengthTier)에 따라 흰색을 첨가하여 최종 명도/발광 느낌을 올립니다.
float ColorBlendAlpha = FMath::Pow(StrengthTier, 3.0f);
ChildData.BodyColor = FMath::Lerp(SaturatedColor, FLinearColor::White, ColorBlendAlpha);

	return ChildData;
}

float APal::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	AController* MyController = GetController();

	// [유기적 수정] CurrentHP -> PalData.CurrentHP
	if (PalData.CurrentHP <= 0.f) return 0.f; // 이미 죽은 상태면 데미지 무시

	if (ActualDamage > 0.f)
	{
		PalData.CurrentHP -= ActualDamage;
		UE_LOG(LogTemp, Warning, TEXT("%s's HP: %f"), *GetName(), PalData.CurrentHP);

		if (PalData.CurrentHP <= 0)
		{
			// 랙돌 처리
			USkeletalMeshComponent* MeshComp = GetMesh();
			if (MeshComp)
			{
				MeshComp->SetCollisionProfileName(TEXT("Ragdoll"));
				MeshComp->SetSimulatePhysics(true);
				MeshComp->WakeAllRigidBodies();
				MeshComp->bBlendPhysics = true;
			}
			// AI 컨트롤러 해제
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
	// [유기적 수정] Level -> PalData.Level
	return BaseXP * FMath::Pow(PalData.Level, Exponent);
}

void APal::GainExperience(float Amount)
{
	// [유기적 수정] CurrentXP -> PalData.CurrentXP
	PalData.CurrentXP += Amount;
	CheckLevelUp();
}

void APal::CheckLevelUp()
{
	// [유기적 수정] CurrentXP -> PalData.CurrentXP
	// 죽지 않았을 때만 레벨업 체크
	while (PalData.CurrentHP > 0 && PalData.CurrentXP >= GetXPToNextLevel())
	{
		PalData.CurrentXP -= GetXPToNextLevel();
		LevelUp();
	}
}

void APal::LevelUp()
{
	// [유기적 수정] Level, bIsBred, BaseStats -> PalData.
	PalData.Level++;

	if (PalData.bIsBred && PalData.Level >= 5)
	{
		SetActorScale3D(FVector(1.0f, 1.0f, 1.0f));
	}

	// 레벨업 시 스탯 상승 (예시)
	PalData.BaseMaxHP += 1.0f;
	PalData.BaseAttack += 2.0f;
	PalData.BaseDefense += 1.0f;

	RecalculateStats();
	PalData.CurrentHP = FinalMaxHP; // 레벨업 시 HP 모두 회복
}

void APal::HandleMasterCombat(AActor* TargetEnemy)
{
	// CCTV 1: 팰이 주인의 신호를 정상적으로 수신했는가?
	UE_LOG(LogTemp, Warning, TEXT("[Pal CCTV 1] %s: Received Combat Signal! Target: %s"),
		*GetName(), TargetEnemy ? *TargetEnemy->GetName() : TEXT("Null"));

	if (!TargetEnemy || PalData.CurrentHP <= 0.f)
	{
		UE_LOG(LogTemp, Error, TEXT("[Pal CCTV] %s: Target is Null or Pal is dead. Aborting."), *GetName());
		return;
	}

	AAIController* AICon = Cast<AAIController>(GetController());
	if (!AICon)
	{
		// 에러: 팰에게 뇌(AI 컨트롤러)가 없는 상태입니다.
		UE_LOG(LogTemp, Error, TEXT("[Pal CCTV] %s: AIController is NULL!"), *GetName());
		return;
	}

	UBlackboardComponent* BlackboardComp = AICon->GetBlackboardComponent();
	if (!BlackboardComp)
	{
		// 에러: AI 컨트롤러는 있는데 블랙보드가 안 켜져 있습니다. (보통 비헤이비어 트리가 실행 안 됐을 때 발생)
		UE_LOG(LogTemp, Error, TEXT("[Pal CCTV] %s: BlackboardComponent is NULL!"), *GetName());
		return;
	}

	// 블랙보드에 타겟을 주입합니다.
	// ★ 주의: "TargetActor"라는 글자가 언리얼 에디터 블랙보드에 만든 Key 이름과 똑같은지 꼭 확인하세요!
	BlackboardComp->SetValueAsObject(TEXT("BattleActor"), TargetEnemy);

	AIState = 3;

	// CCTV 2: 블랙보드 주입 성공 확인
	UE_LOG(LogTemp, Warning, TEXT("[Pal CCTV 2] %s: Successfully set TargetActor to %s in Blackboard!"), *GetName(), *TargetEnemy->GetName());

	// 화면에 초록색 글씨로 띄워줍니다.
	if (GEngine)
	{
		FString DebugMsg = FString::Printf(TEXT("%s -> %s Attack!"), *GetName(), *TargetEnemy->GetName());
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, DebugMsg);
	}
}

void APal::SetMaster(AActor* NewMaster)
{
	// [수정 1] AActor를 ACharacter로 안전하게 형변환(Cast)해서 집어넣습니다!
	PalData.OwnerCharacter = Cast<ACharacter>(NewMaster);

	// 2. 주인이 플레이어가 맞다면 델리게이트를 연결합니다!
	if (APalWorld_StudyCharacter* PlayerMaster = Cast<APalWorld_StudyCharacter>(NewMaster))
	{
		PlayerMaster->OnCombatEngaged.AddDynamic(this, &APal::HandleMasterCombat);
		UE_LOG(LogTemp, Warning, TEXT("[System] %s has recognized %s as MASTER!"), *GetName(), *PlayerMaster->GetName());
	}
}

void APal::RecalculateStats()
{
	float AttackScale_Total = 1.0f;
	float DefenseScale_Total = 1.0f;
	// (필요하다면 HPScale_Total 등 다른 스탯도 추가)

	const TMap<EPalTraitType, FPalTrait>& TraitMap = GetTraitMap();

	// [유기적 수정] Traits -> PalData.Traits
	for (const EPalTraitType& TraitType : PalData.Traits)
	{
		if (const FPalTrait* TraitData = TraitMap.Find(TraitType))
		{
			AttackScale_Total *= TraitData->AttackScale;
			DefenseScale_Total *= TraitData->DefenseScale;
			// (필요하다면 TraitData->HPScale 등 다른 스탯도 반영)
		}
	}

	// [유기적 수정] Base... / IV... -> PalData.Base... / PalData.IV...
	FinalMaxHP = (PalData.BaseMaxHP + PalData.HP_IV); // (특성 스케일링도 추가 가능)
	FinalAttack = (PalData.BaseAttack + PalData.Attack_IV) * AttackScale_Total;
	FinalDefense = (PalData.BaseDefense + PalData.Defense_IV) * DefenseScale_Total;

	// [유기적 수정] CurrentHP -> PalData.CurrentHP
	// 현재 HP가 새로운 최대 HP를 넘지 않도록 조정
	PalData.CurrentHP = FMath::Min(PalData.CurrentHP, FinalMaxHP);
}

