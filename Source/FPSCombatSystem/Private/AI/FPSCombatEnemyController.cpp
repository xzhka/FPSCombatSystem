// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/FPSCombatEnemyController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/Components/FPSCombatHealthComponent.h"
#include "FPSCombatSystem/FPSCombatSystem.h"
#include "GameModes/FPSCombatPlayerState.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"

AFPSCombatEnemyController::AFPSCombatEnemyController()
{
	bWantsPlayerState = true;

	UAIPerceptionComponent* Perception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));
	SetPerceptionComponent(*Perception);

	Sight = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight"));
	Sight->SightRadius = 5000.f;
	Sight->LoseSightRadius = 6000.f;
	Sight->PeripheralVisionAngleDegrees = 80.f;
	Sight->SetMaxAge(1.f);
	Sight->DetectionByAffiliation.bDetectEnemies = true;
	Sight->DetectionByAffiliation.bDetectFriendlies = false;
	Sight->DetectionByAffiliation.bDetectNeutrals = false;

	Perception->ConfigureSense(*Sight);
	Perception->SetDominantSense(Sight->GetSenseImplementation());
}

FGenericTeamId AFPSCombatEnemyController::GetGenericTeamId() const
{
	if (AFPSCombatPlayerState* PS = GetPlayerState<AFPSCombatPlayerState>())
	{
		return PS->GetGenericTeamId();
	}
	return FGenericTeamId::NoTeam;
}

void AFPSCombatEnemyController::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (UAIPerceptionComponent* Perception = GetPerceptionComponent())
	{
		Perception->OnTargetPerceptionUpdated.AddUniqueDynamic(this, &AFPSCombatEnemyController::OnTargetPerceptionUpdate);
	}
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

	if (UAIPerceptionComponent* Perception = GetPerceptionComponent())
	{
		Perception->RequestStimuliListenerUpdate();
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

void AFPSCombatEnemyController::OnTargetPerceptionUpdate(AActor* Actor, FAIStimulus Stimulus)
{
	if (!Actor) return;
	
	if (Stimulus.WasSuccessfullySensed())
	{
		if (GetTeamAttitudeTowards(*Actor) == ETeamAttitude::Hostile)
		{
			UFPSCombatHealthComponent* Health = UFPSCombatHealthComponent::GetHealthComp(Actor);
			if (!Health || Health->IsDeadOrDying()) return;

			if (UBlackboardComponent* BB = GetBlackboardComponent())
			{
				BB->SetValueAsObject(TEXT("TargetActor"), Actor);
			}
		}
	}
	else
	{
		if (UBlackboardComponent* BB = GetBlackboardComponent())
		{
			if (BB->GetValueAsObject(TEXT("TargetActor")) == Actor)
			{
				BB->ClearValue(TEXT("TargetActor"));
			}
		}
	}
}

UFPSCombatAbilitySystemComponent* AFPSCombatEnemyController::GetCombatASC() const
{
	const AFPSCombatPlayerState* PS = GetPlayerState<AFPSCombatPlayerState>();
	return PS ? Cast<UFPSCombatAbilitySystemComponent>(PS->GetAbilitySystemComponent()) : nullptr;
}
