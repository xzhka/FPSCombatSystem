// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/FPSCombatGameplayAbilityUpdraft.h"

#include "Characters/FPSCombatMovementComp.h"

UFPSCombatGameplayAbilityUpdraft::UFPSCombatGameplayAbilityUpdraft()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	FGameplayTagContainer Tags;
	Tags.AddTag(FPSCombatGameplayTags::Ability_Moving_AirborneSource_Updraft);
	SetAssetTags(Tags);
}

void UFPSCombatGameplayAbilityUpdraft::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!IsActive())
	{
		return;
	}
	
	if (!CommitAbilityCost(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	Updraft();

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UFPSCombatGameplayAbilityUpdraft::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UFPSCombatGameplayAbilityUpdraft::Updraft()
{
	if (UFPSCombatMovementComp* MovementComp = UFPSCombatMovementComp::GetMovementComp(GetAvatarActorFromActorInfo()))
	{
		MovementComp->Updraft(Distance);
	}
	
	if (AbilityTagGrantEffect)
	{
		UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(AbilityTagGrantEffect, GetAbilityLevel());
		
		if (SpecHandle.IsValid())
		{
			AirborneEffectHandle = ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, SpecHandle);

			ASC->OnGameplayEffectRemoved_InfoDelegate(AirborneEffectHandle)->AddUObject(this, &UFPSCombatGameplayAbilityUpdraft::HandleUpdraftSourceChanged);
			
		}
	}
}

void UFPSCombatGameplayAbilityUpdraft::HandleUpdraftSourceChanged(const FGameplayEffectRemovalInfo& RemovalInfo)
{
	ApplyCooldown(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo);
}
