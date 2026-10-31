// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/FPSCombatGameState.h"

AFPSCombatGameState::AFPSCombatGameState()
{
	SpawnManagerComponent = CreateDefaultSubobject<UFPSCombatSpawningManagerComponent>(TEXT("SpawnManager"));
	SpawnerComponent = CreateDefaultSubobject<UFPSCombatAISpawnerComponent>(TEXT("SpawnerComponent"));
}
