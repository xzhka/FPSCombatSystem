// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/FPSCombatEnemyController.h"

#include "Characters/Components/FPSCombatHealthComponent.h"
#include "FPSCombatSystem/FPSCombatSystem.h"
#include "GameModes/FPSCombatPlayerState.h"

AFPSCombatEnemyController::AFPSCombatEnemyController()
{
	bWantsPlayerState = true;
}

FGenericTeamId AFPSCombatEnemyController::GetGenericTeamId() const
{
	if (AFPSCombatPlayerState* PS = GetPlayerState<AFPSCombatPlayerState>())
	{
		return PS->GetGenericTeamId();
	}
	return FGenericTeamId::NoTeam;
}

void AFPSCombatEnemyController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (UFPSCombatHealthComponent* HealthComponent = UFPSCombatHealthComponent::GetHealthComp(InPawn))
	{
		HealthComponent->OnDeathStarted.AddUniqueDynamic(this, &AFPSCombatEnemyController::OnDeathStarted);
	}
	
	if (IsValid(BehaviorTree))
	{
		RunBehaviorTree(BehaviorTree);
	}

	if (IsValid(BrainComponent))
	{
		BrainComponent->StartLogic();
	}
}

void AFPSCombatEnemyController::OnUnPossess()
{
	if (APawn* MyOldPawn = GetPawn())
	{
		if (UFPSCombatHealthComponent* HealthComponent = UFPSCombatHealthComponent::GetHealthComp(MyOldPawn))
		{
			HealthComponent->OnDeathStarted.RemoveDynamic(this, &AFPSCombatEnemyController::OnDeathStarted);
		}
	}
	Super::OnUnPossess();	
}

void AFPSCombatEnemyController::OnDeathStarted(AActor* OwningActor)
{
	if (BrainComponent)
	{
		BrainComponent->StopLogic(TEXT("Pawn Dead"));
	}
}

UFPSCombatAbilitySystemComponent* AFPSCombatEnemyController::GetCombatASC() const
{
	const AFPSCombatPlayerState* PS = GetPlayerState<AFPSCombatPlayerState>();
	return PS ? Cast<UFPSCombatAbilitySystemComponent>(PS->GetAbilitySystemComponent()) : nullptr;
}
