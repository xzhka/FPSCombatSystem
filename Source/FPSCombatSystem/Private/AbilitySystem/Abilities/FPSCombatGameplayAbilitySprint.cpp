// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/FPSCombatGameplayAbilitySprint.h"

#include "AbilitySystemComponent.h"
#include "FPSCombatSystem/FPSCombatGameplayTags.h"

UFPSCombatGameplayAbilitySprint::UFPSCombatGameplayAbilitySprint()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	RemovedOnTags.AddTag(FPSCombatGameplayTags::State_Moving_MovingForward);
	RemovedOnTags.AddTag(FPSCombatGameplayTags::State_Moving_Walking);
	
}

void UFPSCombatGameplayAbilitySprint::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
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
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}
	
	EffectSpecApply(SprintGrantedEffectsClass, SprintGrantedEffectHandle);

	
	
}

void UFPSCombatGameplayAbilitySprint::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)	
{
	
	if (bWasCancelled)
	{
		if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
		{
			if (ASC->HasMatchingGameplayTag(FPSCombatGameplayTags::State_Stamina_OutOfStamina))
			{
				ApplyCooldown(Handle, ActorInfo, ActivationInfo);
			}
		}
	}
	EffectSpecRemove(SprintGrantedEffectHandle);
	
	SprintGrantedEffectHandle.Empty();
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UFPSCombatGameplayAbilitySprint::InputReleased(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UFPSCombatGameplayAbilitySprint::CommitExecute(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	ApplyCost(Handle, ActorInfo, ActivationInfo);
}