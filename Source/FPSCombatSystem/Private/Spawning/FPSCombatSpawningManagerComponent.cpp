// Fill out your copyright notice in the Description page of Project Settings.


#include "Spawning/FPSCombatSpawningManagerComponent.h"

#include "EngineUtils.h"
#include "AI/FPSCombatEnemyController.h"
#include "GameModes/FPSCombatGameMode.h"
#include "Spawning/FPSCombatPlayerStart.h"

AActor* UFPSCombatSpawningManagerComponent::ChoosePlayerStart(AController* InController) const
{
	if (!InController) return nullptr;

	const IGenericTeamAgentInterface* TeamAgent = Cast<IGenericTeamAgentInterface>(InController);
	if (!TeamAgent) return nullptr;

	const FGenericTeamId ControllerTeam = TeamAgent->GetGenericTeamId();

	const APawn* FitPawn = GetPawnToFit(InController);
	
	TArray<AActor*> AllStarts;
	TArray<AActor*> FreeStarts;

	for (TActorIterator<AFPSCombatPlayerStart> It(GetWorld()); It; ++It)
	{
		if (It->GetTeamId() != ControllerTeam) continue;

		AllStarts.Add(*It);
		if (!IsBlockedStart(FitPawn,*It))
		{
			FreeStarts.Add(*It);
		}
	}


	const TArray<AActor*>& PawnPool = FreeStarts.IsEmpty() ? AllStarts : FreeStarts;
	return PawnPool.IsEmpty() ? nullptr : PawnPool[FMath::RandRange(0, PawnPool.Num() - 1)];
}

const APawn* UFPSCombatSpawningManagerComponent::GetPawnToFit(AController* Controller) const
{
	AFPSCombatGameMode* GameMode = GetGameMode<AFPSCombatGameMode>();
	const UClass* PawnClass = GameMode ? GameMode->GetDefaultPawnClassForController(Controller) : nullptr;
	return PawnClass ? PawnClass->GetDefaultObject<APawn>() : nullptr;
}

bool UFPSCombatSpawningManagerComponent::IsBlockedStart(const APawn* ActorToFit, AActor* Start) const
{
	return ActorToFit && GetWorld()->EncroachingBlockingGeometry(ActorToFit, Start->GetActorLocation(), Start->GetActorRotation());
}
