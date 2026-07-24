// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Aim/FPSCombatGameplayAbilityAim.h"

void UFPSCombatGameplayAbilityAim::InputPressed(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
