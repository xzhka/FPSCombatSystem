// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Fire/FPSCombatGameplayAbilityReload.h"

#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "Weapons/FPSCombatRangedWeaponInstance.h"

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
}

void UFPSCombatGameplayAbilityReload::GrantReloadAmmo() const
{
	UFPSCombatRangedWeaponInstance* WeaponInstance = GetWeaponInstance();
	UE_LOG(LogTemp, Warning, TEXT("[%s] GrantReloadAmmo called. WeaponInstance = %s"),
			CurrentActorInfo && CurrentActorInfo->IsNetAuthority() ? TEXT("SERVER") : TEXT("CLIENT"),
			WeaponInstance ? TEXT("valid") : TEXT("NULL"));
	if (CurrentActorInfo && CurrentActorInfo->IsNetAuthority())
	{
		if (WeaponInstance)
		{
			WeaponInstance->ReloadAmmo();
		}
	}
}

UFPSCombatRangedWeaponInstance* UFPSCombatGameplayAbilityReload::GetWeaponInstance() const
{
	if (FGameplayAbilitySpec* Spec = UGameplayAbility::GetCurrentAbilitySpec())
	{
		return Cast<UFPSCombatRangedWeaponInstance>(Spec->SourceObject.Get());
	}
	return nullptr;
}