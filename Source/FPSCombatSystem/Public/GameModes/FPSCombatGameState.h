// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/FPSCombatAISpawnerComponent.h"
#include "GameFramework/GameState.h"
#include "Spawning/FPSCombatSpawningManagerComponent.h"
#include "FPSCombatGameState.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, meta=(AFPSCombatGameState))
class FPSCOMBATSYSTEM_API AFPSCombatGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	AFPSCombatGameState();

private:
	UPROPERTY(VisibleAnywhere, Category="Spawning")
	TObjectPtr<UFPSCombatSpawningManagerComponent> SpawnManagerComponent;

	UPROPERTY(VisibleAnywhere, Category="Spawning")
	TObjectPtr<UFPSCombatAISpawnerComponent> SpawnerComponent;
};
