// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/FPSCombatGameplayAbilityInteract.h"

UFPSCombatGameplayAbilityInteract::UFPSCombatGameplayAbilityInteract()
{
	ActivationPolicy = EFPSCombatAbilityActivationPolicy::OnSpawn;

	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}
