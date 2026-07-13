// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/FPSCombatBaseGameplayAbility.h"

#include "Abilities/Tasks/AbilityTask_WaitGameplayTag.h"
#include "AbilitySystem/FPSCombatAbilitySystemComponent.h"
#include "AbilitySystem/Tasks/AbilityTask_WaitCancelTags.h"
#include "FPSCombatSystem/FPSCombatGameplayTags.h"

UFPSCombatBaseGameplayAbility::UFPSCombatBaseGameplayAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	ActivationBlockedTags.AddTag(FPSCombatGameplayTags::State_Death);
	
}

const FGameplayTagContainer* UFPSCombatBaseGameplayAbility::GetCooldownTags() const
{
	FGameplayTagContainer* MutableCooldownTags = const_cast<FGameplayTagContainer*>(&TempCooldownTags);
	MutableCooldownTags->Reset();
	const FGameplayTagContainer* ParentTag = Super::GetCooldownTags();
	if (ParentTag)
	{
		MutableCooldownTags->AppendTags(*ParentTag);
	}
	MutableCooldownTags->AppendTags(CooldownTags);
	
	return MutableCooldownTags;
}

void UFPSCombatBaseGameplayAbility::ApplyCooldown(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	UGameplayEffect* CooldownGE = GetCooldownGameplayEffect();
	if (CooldownGE)
	{
		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(CooldownGE->GetClass(), GetAbilityLevel());
		SpecHandle.Data->DynamicGrantedTags.AppendTags(CooldownTags);
		if (ensureMsgf(SetByCallerTag.IsValid(), TEXT("%s: CooldownSetByCallerTag not set"), *GetName()))
		{
			SpecHandle.Data.Get()->SetSetByCallerMagnitude(SetByCallerTag, CooldownDuration.GetValueAtLevel(GetAbilityLevel()));
		}
		
		ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
	}
}

bool UFPSCombatBaseGameplayAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	if (!ActivationTagQuery.IsEmpty())
	{
		const UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
		if (!ActivationTagQuery.Matches(ASC->GetOwnedGameplayTags()))
		{
			return false;
		}
	}
	return true;
}

void UFPSCombatBaseGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                    const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                    const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UAbilityTask_WaitCancelTags* WaitCancelTags = UAbilityTask_WaitCancelTags::WaitCancelTags(this, AddedOnTags, RemovedOnTags);
	WaitCancelTags->OnCancelTagTriggered.AddDynamic(this, &UFPSCombatBaseGameplayAbility::HandleCancelTagTriggered);
	WaitCancelTags->ReadyForActivation();
}

void UFPSCombatBaseGameplayAbility::HandleCancelTagTriggered()
{
	CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
}

void UFPSCombatBaseGameplayAbility::EffectSpecApply(TArray<TSubclassOf<UGameplayEffect>> GrantedEffectsClass,
	TArray<FActiveGameplayEffectHandle>& GrantedEffectHandle,
	FGameplayTag DurationSetByCallerTag,
	float DurationValue)
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (ASC)
	{
		const FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
		for (const TSubclassOf<UGameplayEffect>& EffectClass : GrantedEffectsClass)
		{
			FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(EffectClass, GetAbilityLevel(), Context);
			if (SpecHandle.IsValid())
			{
				if (DurationSetByCallerTag.IsValid())
				{
					SpecHandle.Data->SetSetByCallerMagnitude(DurationSetByCallerTag, DurationValue);
				}
				GrantedEffectHandle.Add(ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get()));
			}
		}
	}
}

void UFPSCombatBaseGameplayAbility::EffectSpecRemove(TArray<FActiveGameplayEffectHandle> GrantedEffectHandle)
{
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		for (const FActiveGameplayEffectHandle& EffectHandle : GrantedEffectHandle)
		{
			ASC->RemoveActiveGameplayEffect(EffectHandle);
		}
	}

}
