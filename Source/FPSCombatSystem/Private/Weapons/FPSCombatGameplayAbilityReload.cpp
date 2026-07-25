// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/FPSCombatGameplayAbilityReload.h"

#include "Abilities/Tasks/AbilityTask_WaitDelay.h"

UFPSCombatGameplayAbilityReload::UFPSCombatGameplayAbilityReload()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

bool UFPSCombatGameplayAbilityReload::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
    const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}
	
	if (const FGameplayAbilitySpec* Spec = ActorInfo->AbilitySystemComponent->FindAbilitySpecFromHandle(Handle))
	{
		if (const UFPSCombatRangedWeaponInstance* RangedWeapon = Cast<UFPSCombatRangedWeaponInstance>(Spec->SourceObject.Get()))
		{
			const bool bCanReload = RangedWeapon->CanReload();
			return bCanReload;
		}
	}
	return false;
}

void UFPSCombatGameplayAbilityReload::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	UFPSCombatRangedWeaponInstance* WeaponData = GetWeaponInstance();
	if (!WeaponData || !WeaponData->CanReload())
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	WaitDelayTask = UAbilityTask_WaitDelay::WaitDelay(this, WeaponData->GetWeaponDefinition()->ReloadDuration);
	WaitDelayTask->OnFinish.AddDynamic(this, &UFPSCombatGameplayAbilityReload::OnReloadFinished);
	WaitDelayTask->ReadyForActivation();
}

UFPSCombatRangedWeaponInstance* UFPSCombatGameplayAbilityReload::GetWeaponInstance() const
{
	if (FGameplayAbilitySpec* Spec = UGameplayAbility::GetCurrentAbilitySpec())
	{
		return Cast<UFPSCombatRangedWeaponInstance>(Spec->SourceObject.Get());
	}
	return nullptr;
}

void UFPSCombatGameplayAbilityReload::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	if (WaitDelayTask)
	{
		WaitDelayTask->EndTask();
		WaitDelayTask = nullptr;
	}
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UFPSCombatGameplayAbilityReload::OnReloadFinished()
{
	if (CurrentActorInfo->IsNetAuthority())
	{
		UFPSCombatRangedWeaponInstance* WeaponInstance = GetWeaponInstance();
		if (WeaponInstance)
		{
			WeaponInstance->ReloadAmmo();
		}
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
	else
	{
		// Client don`t end the ability by itself.
		// We let the server to authoritative and replicate down ability instead.
		WaitDelayTask = nullptr;
	}
}