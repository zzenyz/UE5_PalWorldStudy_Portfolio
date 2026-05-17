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

	if (!ControlledPal->PalData.AttackMontage)
	{
		return EBTNodeResult::Failed;
	}

	USkeletalMeshComponent* MeshComp = ControlledPal->GetMesh();
	if (!MeshComp) return EBTNodeResult::Failed;

	UAnimInstance* AnimInstance = MeshComp->GetAnimInstance();
	if (!AnimInstance) return EBTNodeResult::Failed;

	CurrentPlayingMontage = ControlledPal->PalData.AttackMontage;
	AnimInstance->Montage_Play(CurrentPlayingMontage, 1.0f);
	bIsAttacking = true;

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

	if (!AnimInstance->Montage_IsPlaying(CurrentPlayingMontage))
	{
		bIsAttacking = false;
		CurrentPlayingMontage = nullptr;
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}