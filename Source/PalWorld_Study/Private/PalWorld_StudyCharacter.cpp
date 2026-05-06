// Copyright Epic Games, Inc. All Rights Reserved.

#include "PalWorld_StudyCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "PalStudyGameInstance.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// APalWorld_StudyCharacter

APalWorld_StudyCharacter::APalWorld_StudyCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void APalWorld_StudyCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Add Input Mapping Context (only if valid)
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (PlayerController->GetLocalPlayer() && DefaultMappingContext)
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
				ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
			{
				Subsystem->AddMappingContext(DefaultMappingContext, 0);
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("DefaultMappingContext is NULL or LocalPlayer not found!"));
		}
	}

	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{
		PC->bShowMouseCursor = false;

		FInputModeGameOnly GameOnlyInputMode;
		PC->SetInputMode(GameOnlyInputMode);
	}

	if (UPalStudyGameInstance* GI = Cast<UPalStudyGameInstance>(GetGameInstance()))
	{
		if (GI->bShouldLoadData)
		{
			// 로드 플래그가 켜져 있으면 저장된 데이터를 덮어씌웁니다.
			GI->LoadPlayerData();

			// 한 번 로드했으면 플래그를 꺼줍니다. (안전장치)
			GI->bShouldLoadData = false;
		}
	}
}

float APalWorld_StudyCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	// ------------------ 디버깅 CCTV 시작 ------------------
	// 1. 출력 로그(Output Log)에 노란색으로 크게 찍습니다.
	UE_LOG(LogTemp, Warning, TEXT("!!! 플레이어 피격 판정 들어옴 !!! 데미지: %f, 때린 놈: %s"),
		DamageAmount, DamageCauser ? *DamageCauser->GetName() : TEXT("Null"));

	// 2. 게임 화면 좌측 상단에 빨간색 글씨로 3초 동안 띄웁니다.
	if (GEngine)
	{
		FString DebugMsg = FString::Printf(TEXT("아얏! 플레이어 맞음! 때린 놈: %s"), DamageCauser ? *DamageCauser->GetName() : TEXT("Null"));
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, DebugMsg);
	}
	// ------------------ 디버깅 CCTV 끝 ------------------

	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	// 데미지가 0보다 클 때만 팰들을 부릅니다.
	if (DamageCauser && ActualDamage > 0.f)
	{
		OnCombatEngaged.Broadcast(DamageCauser);
	}

	return ActualDamage;
}

//////////////////////////////////////////////////////////////////////////
// Input

void APalWorld_StudyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APalWorld_StudyCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APalWorld_StudyCharacter::Look);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void APalWorld_StudyCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void APalWorld_StudyCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}