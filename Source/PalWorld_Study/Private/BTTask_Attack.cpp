#include "BTTask_Attack.h"
#include "AIController.h"
#include "Pal.h"
#include "Animation/AnimInstance.h"

UBTTask_Attack::UBTTask_Attack()
{
	NodeName = TEXT("Attack Dynamic");
	bNotifyTick = true;
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return EBTNodeResult::Failed;

	APal* ControlledPal = Cast<APal>(AIController->GetPawn());
	if (!ControlledPal) return EBTNodeResult::Failed;

	// CCTV 1: 한글 깨짐 방지를 위해 영어로 출력
	UE_LOG(LogTemp, Warning, TEXT("--- BTTask_Attack Executed! Pal Name: %s ---"), *ControlledPal->GetName());

	if (!ControlledPal->PalData.AttackMontage)
	{
		UE_LOG(LogTemp, Error, TEXT("Error 1: AttackMontage is NULL! (Set it in Blueprint or DataTable)"));
		return EBTNodeResult::Failed;
	}

	USkeletalMeshComponent* MeshComp = ControlledPal->GetMesh();
	if (!MeshComp)
	{
		UE_LOG(LogTemp, Error, TEXT("Error 2: Skeletal Mesh is NULL!"));
		return EBTNodeResult::Failed;
	}

	UAnimInstance* AnimInstance = MeshComp->GetAnimInstance();
	if (!AnimInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("Error 3: AnimInstance is NULL! (Assign AnimBP in Mesh Component)"));
		return EBTNodeResult::Failed;
	}

	// 1. 몽타주 플레이
	CurrentPlayingMontage = ControlledPal->PalData.AttackMontage;
	AnimInstance->Montage_Play(CurrentPlayingMontage, 1.0f);
	bIsAttacking = true;

	// CCTV 2
	UE_LOG(LogTemp, Warning, TEXT("--- Calling ExecuteBPAttack from C++ ---"));

	// 2. 블루프린트 공격 로직 즉시 호출!
	ControlledPal->ExecuteBPAttack();

	return EBTNodeResult::InProgress;
}

void UBTTask_Attack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if (!bIsAttacking || !CurrentPlayingMontage) return;

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return;

	APal* ControlledPal = Cast<APal>(AIController->GetPawn());
	if (!ControlledPal) return;

	UAnimInstance* AnimInstance = ControlledPal->GetMesh()->GetAnimInstance();
	if (!AnimInstance) return;

	// 저장해둔 그 몽타주가 끝났는지 확인
	if (!AnimInstance->Montage_IsPlaying(CurrentPlayingMontage))
	{
		bIsAttacking = false;
		CurrentPlayingMontage = nullptr; // 비워주기

		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}