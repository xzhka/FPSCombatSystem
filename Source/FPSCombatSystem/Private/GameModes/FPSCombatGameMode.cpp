// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/FPSCombatGameMode.h"

#include "Characters/FPSCombatCharacter.h"
#include "Input/FPSCombatPlayerController.h"

AFPSCombatGameMode::AFPSCombatGameMode()
{
	DefaultPawnClass = AFPSCombatCharacter::StaticClass();
	
}
