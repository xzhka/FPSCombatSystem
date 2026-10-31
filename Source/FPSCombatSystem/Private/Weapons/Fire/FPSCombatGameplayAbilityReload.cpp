// FPS Combat project


#include "Weapons/Fire/FPSCombatGameplayAbilityReload.h"

#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "FPSCombatSystem/FPSCombatGameplayTags.h"
#include "Weapons/FPSCombatRangedWeaponInstance.h"

UFPSCombatGameplayAbilityReload::UFPSCombatGameplayAbilityReload()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	FAbilityTriggerData TriggerData;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	TriggerData.TriggerTag = FPSCombatGameplayTags::GameplayEvent_Weapon_Reload;
	
	AbilityTriggers.Add(TriggerData);
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