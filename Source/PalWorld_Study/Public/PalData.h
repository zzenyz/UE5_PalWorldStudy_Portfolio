#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "PalData.generated.h"

class APal;
class ACharacter;

// Pal Trait 
UENUM(BlueprintType)
enum class EPalTraitType : uint8
{
	Brave           UMETA(DisplayName = "Brave"),
	Ferocious       UMETA(DisplayName = "Ferocious"),
	HardSkin        UMETA(DisplayName = "Hard Skin"),
	ToughBody       UMETA(DisplayName = "Tough Body"),
	DiamondBody     UMETA(DisplayName = "Diamond Body"),
	Impetuous       UMETA(DisplayName = "Impetuous"),
	LevelHeaded     UMETA(DisplayName = "Level-Headed"),
	Demon           UMETA(DisplayName = "Demon"),
	Legendary       UMETA(DisplayName = "Legendary"),
	Vampire         UMETA(DisplayName = "Vampire"),

	Swift           UMETA(DisplayName = "Swift"),
	Runner          UMETA(DisplayName = "Runner"),
	Nimble          UMETA(DisplayName = "Nimble"),
	HealthyConstitution UMETA(DisplayName = "Healthy Constitution"),
	AbundantEnergy    UMETA(DisplayName = "Abundant Energy"),
	PerpetualMotion   UMETA(DisplayName = "Perpetual Motion"),

	Serious         UMETA(DisplayName = "Serious"),
	Artisan         UMETA(DisplayName = "Artisan"),
	MasterTechnician  UMETA(DisplayName = "Master Technician"),
	WorkSlave       UMETA(DisplayName = "Work Slave"),
	DietExpert      UMETA(DisplayName = "Diet Expert"),
	Abstinence      UMETA(DisplayName = "Abstinence"),
	PositiveThinker   UMETA(DisplayName = "Positive Thinker"),
	Workaholic      UMETA(DisplayName = "Workaholic"),
	UnwaveringHeart   UMETA(DisplayName = "Unwavering Heart"),
	LoggingForeman    UMETA(DisplayName = "Logging Foreman"),
	MiningForeman     UMETA(DisplayName = "Mining Foreman"),
	Motivation      UMETA(DisplayName = "Motivation"),
	Philanthropist  UMETA(DisplayName = "Philanthropist"),
	Nocturnal       UMETA(DisplayName = "Nocturnal"),
	Rare            UMETA(DisplayName = "Rare"),

	Slacker         UMETA(DisplayName = "Slacker"),
	WeakGrip        UMETA(DisplayName = "Weak Grip"),
	BrittleBones    UMETA(DisplayName = "Brittle Bones"),
	Coward          UMETA(DisplayName = "Coward"),
	Pacifist        UMETA(DisplayName = "Pacifist"),
	Bully           UMETA(DisplayName = "Bully"),
	Sadist          UMETA(DisplayName = "Sadist"),
	Masochist       UMETA(DisplayName = "Masochist"),
	Hooligan        UMETA(DisplayName = "Hooligan"),
	Musclehead      UMETA(DisplayName = "Musclehead"),
	Clumsy          UMETA(DisplayName = "Clumsy"),
	Conceited       UMETA(DisplayName = "Conceited"),
	Glutton         UMETA(DisplayName = "Glutton"),
	BlackHoleStomach  UMETA(DisplayName = "Black Hole Stomach"),
	Unstable        UMETA(DisplayName = "Unstable"),
	Destructive     UMETA(DisplayName = "Destructive"),

	AssaultCommander    UMETA(DisplayName = "Assault Commander"),
	BulwarkStrategist   UMETA(DisplayName = "Bulwark Strategist")
};

USTRUCT(BlueprintType)
struct FPalTrait
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) float HPScale;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float AttackScale;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float DefenseScale;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float MovementSpeedScale;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float MaxStaminaScale;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float WorkSpeedScale;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float HungerDepletionScale;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float SanityDepletionScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) float SkillCooldownReduction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float PlayerAttackBuff;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float PlayerDefenseBuff;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float PlayerLoggingEfficiency;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float PlayerMiningEfficiency;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float PlayerWorkSpeedBuff;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bHasLifesteal;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bWorksAtNight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bIncreasesEggProduction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) float TraitScore;

	FPalTrait()
		: Name(TEXT("Default")), HPScale(1.0f), AttackScale(1.0f), DefenseScale(1.0f), MovementSpeedScale(1.0f),
		MaxStaminaScale(1.0f), WorkSpeedScale(1.0f), HungerDepletionScale(1.0f),
		SanityDepletionScale(1.0f), SkillCooldownReduction(0.0f), PlayerAttackBuff(0.0f),
		PlayerDefenseBuff(0.0f), PlayerLoggingEfficiency(0.0f), PlayerMiningEfficiency(0.0f),
		PlayerWorkSpeedBuff(0.0f), bHasLifesteal(false), bWorksAtNight(false),
		bIncreasesEggProduction(false), TraitScore(1.0f)
	{
	}

	FPalTrait(float InAtk, float InDef, float InMove, float InStamina, float InWork, float InHunger, float InSanity, float InCooldown,
		float InPlayerAtk, float InPlayerDef, float InPlayerLog, float InPlayerMine, float InPlayerWork,
		bool InLifesteal, bool InNightWork, bool InEgg, float InScore)
		: Name(TEXT("")), HPScale(1.0f), AttackScale(InAtk), DefenseScale(InDef), MovementSpeedScale(InMove),
		MaxStaminaScale(InStamina), WorkSpeedScale(InWork), HungerDepletionScale(InHunger),
		SanityDepletionScale(InSanity), SkillCooldownReduction(InCooldown), PlayerAttackBuff(InPlayerAtk),
		PlayerDefenseBuff(InPlayerDef), PlayerLoggingEfficiency(InPlayerLog), PlayerMiningEfficiency(InPlayerMine),
		PlayerWorkSpeedBuff(InPlayerWork), bHasLifesteal(InLifesteal), bWorksAtNight(InNightWork),
		bIncreasesEggProduction(InEgg), TraitScore(InScore)
	{
	}

	bool operator==(const FPalTrait& Other) const { return Name == Other.Name; }
};

USTRUCT(BlueprintType)
struct FPalData : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pal Data")
	TSubclassOf<APal> PalClass; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pal Data")
	TSoftObjectPtr<UTexture2D> PalIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pal Data|Animation")
	class UAnimMontage* AttackMontage = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pal Data")
	FString PalName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pal Data")
	int32 Level;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pal Data")
	float CurrentXP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pal Data")
	TArray<EPalTraitType> Traits; 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pal Data")
	FLinearColor BodyColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pal Data")
	float CurrentHP;

	// ====== 기본 스탯 ======
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pal Data")
	float BaseMaxHP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pal Data")
	float BaseAttack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pal Data")
	float BaseDefense;

	// ====== 개체값 (IV) ======
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pal Data")
	float HP_IV;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pal Data")
	float Attack_IV;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pal Data")
	float Defense_IV;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pal Data")
	bool bIsBred;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pal Data")
	bool bIsCaptured;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ACharacter* OwnerCharacter = nullptr;
};

