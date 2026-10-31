// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/FPSCombatAISpawnerComponent.h"

#include "GameModes/FPSCombatGameMode.h"

void UFPSCombatAISpawnerComponent::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		SpawnServerAI();
	}
}

void UFPSCombatAISpawnerComponent::SpawnServerAI()
{
	if (AIClass == nullptr)
	{
		return;
	}

	for (int32 i = 0; i < DesiredBotsToCreate; i++)
	{
		SpawnOneAIBot();	
	}
}

void UFPSCombatAISpawnerComponent::SpawnOneAIBot()
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.ObjectFlags |= RF_Transient;

	AAIController* SpawnController = GetWorld()->SpawnActor<AAIController>(AIClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
	if (SpawnController != nullptr)
	{
		AFPSCombatGameMode* GameMode = GetGameMode<AFPSCombatGameMode>();
		check(GameMode);

		GameMode->DispatchPostLogin(SpawnController);
		GameMode->RestartPlayer(SpawnController);

		SpawnedBotList.Add(SpawnController);
	}
}

void UFPSCombatAISpawnerComponent::RemoveOneAIBot()
{
	if (SpawnedBotList.Num() > 0)
	{
		const int32 BotToRemoveIndex = FMath::RandRange(0, SpawnedBotList.Num()-1);
		
		AAIController* RemovedController = SpawnedBotList[BotToRemoveIndex];
		SpawnedBotList.RemoveAtSwap(BotToRemoveIndex);

		if (RemovedController != nullptr)
		{
			if (APawn* Pawn = RemovedController->GetPawn())
			{
				Pawn->Destroy();
			}
			RemovedController->Destroy();
		}
	}
}
