// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/FPSCombatGameplayAbilityDeath.h"

#include "AbilitySystem/FPSCombatAbilitySystemComponent.h"
#include "Characters/Components/FPSCombatHealthComponent.h"
#include "FPSCombatSystem/FPSCombatGameplayTags.h"

UFPSCombatGameplayAbilityDeath::UFPSCombatGameplayAbilityDeath()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;


	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		FAbilityTriggerData TriggerData;
		TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
		TriggerData.TriggerTag = FPSCombatGameplayTags::State_Death;
		
		AbilityTriggers.Add(TriggerData);
	}
	
}


void UFPSCombatGameplayAbilityDeath::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                     const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                     const FGameplayEventData* TriggerEventData)
{
	check(ActorInfo);

	UFPSCombatAbilitySystemComponent* ASC = CastChecked<UFPSCombatAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo());

	ASC->CancelAbilities();

	SetCanBeCanceled(false);

	
	StartDeath();


	
	
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UFPSCombatGameplayAbilityDeath::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	check(ActorInfo);
	
	FinishDeath();
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UFPSCombatGameplayAbilityDeath::StartDeath()
{
	if (UFPSCombatHealthComponent* HC = UFPSCombatHealthComponent::GetHealthComp(GetAvatarActorFromActorInfo()))
	{
		if (HC->GetDeathState() == EDeathState::NotDead)
		{
			HC->DeathStarted();
		}
	}
}

void UFPSCombatGameplayAbilityDeath::FinishDeath()
{
	if (UFPSCombatHealthComponent* HC = UFPSCombatHealthComponent::GetHealthComp(GetAvatarActorFromActorInfo()))
	{
		if (HC->GetDeathState() == EDeathState::DeathStarted)
		{
			HC->DeathEnded();
		}
	}
}

