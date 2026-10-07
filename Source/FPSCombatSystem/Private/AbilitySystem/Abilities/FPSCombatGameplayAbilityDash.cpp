// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/FPSCombatGameplayAbilityDash.h"

#include "Characters/FPSCombatMovementComp.h"
#include "FPSCombatSystem/FPSCombatGameplayTags.h"

UFPSCombatGameplayAbilityDash::UFPSCombatGameplayAbilityDash()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	SetByCallerTag = FPSCombatGameplayTags::SetByCaller_Cooldown_Duration;

	FGameplayTagContainer Tags;
	Tags.AddTag(FPSCombatGameplayTags::Ability_Moving_Dash);
	SetAssetTags(Tags);
	
}

void UFPSCombatGameplayAbilityDash::CommitExecute(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	ApplyCost(Handle, ActorInfo, ActivationInfo);
}

void UFPSCombatGameplayAbilityDash::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                    const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                    const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!IsActive())
	{
		return;
	}
	
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	Dash();
	
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UFPSCombatGameplayAbilityDash::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	ApplyCooldown(Handle, ActorInfo, ActivationInfo);
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UFPSCombatGameplayAbilityDash::Dash()
{
	UAbilitySystemComponent* AbilityASC = GetAbilitySystemComponentFromActorInfo();
	
	if (UFPSCombatMovementComp* MovementComp = UFPSCombatMovementComp::GetMovementComp(GetAvatarActorFromActorInfo()))
	{
		MovementComp->Dash(Strength, Duration);
	}

	// On authority remove current tag effect
	// then apply it manually by duration
	if (CurrentActorInfo->IsNetAuthority())
	{
		EffectSpecRemove(DashGrantedEffectHandle);
		DashGrantedEffectHandle.Reset();
	}

	EffectSpecApply({DashGrantedEffectsClass}, DashGrantedEffectHandle, FPSCombatGameplayTags::SetByCaller_Dash_Duration, Duration);
	
}