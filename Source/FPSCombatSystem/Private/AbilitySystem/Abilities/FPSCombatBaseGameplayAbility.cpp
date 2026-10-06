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

	ActivationPolicy = EFPSCombatAbilityActivationPolicy::OnInputTriggered;
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

bool UFPSCombatBaseGameplayAbility::DoesAbilitySatisfyTagRequirements(
	const UAbilitySystemComponent& AbilitySystemComponent, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::DoesAbilitySatisfyTagRequirements(AbilitySystemComponent, SourceTags, TargetTags,
	                                                OptionalRelevantTags))
	{
		return false;
	}

	const UFPSCombatAbilitySystemComponent* ASC = CastChecked<UFPSCombatAbilitySystemComponent>(&AbilitySystemComponent);
	UFPSCombatTagsRelationshipMapping* Mapping = ASC ? ASC->GetRelationshipMapping() : nullptr;

	if (!Mapping) return true;

	FGameplayTagContainer Required, Blocked;

	Mapping->GetRequiredAndBlockedTags(GetAssetTags(), &Required, &Blocked);

	if (Blocked.Num() && ASC->HasAnyMatchingGameplayTags(Blocked)) return false;
	if (Required.Num() && !(ASC->HasAnyMatchingGameplayTags(Required))) return false;
	
	return true;
}

void UFPSCombatBaseGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	TryActivateAbilityOnSpawn(ActorInfo, Spec);
}

void UFPSCombatBaseGameplayAbility::TryActivateAbilityOnSpawn(const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilitySpec& Spec) const
{
	if (ActorInfo && !Spec.IsActive() && (ActivationPolicy == EFPSCombatAbilityActivationPolicy::OnSpawn))
	{
		UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
		AActor* AvatarActor = ActorInfo->AvatarActor.Get();
		if (ASC && AvatarActor && !AvatarActor->GetTearOff() && (AvatarActor->GetLifeSpan() <= 0.f))
		{
			const bool bIsLocalExecution = (NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::LocalPredicted) || (NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::LocalOnly);
			const bool bIsServerExecution = (NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::ServerInitiated) || (NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::ServerOnly);

			const bool bClientShouldActivate = bIsLocalExecution && ActorInfo->IsLocallyControlled();
			const bool bServerShouldActivate = bIsServerExecution && ActorInfo->IsNetAuthority();
			if (bClientShouldActivate || bServerShouldActivate)
			{
				ASC->TryActivateAbility(Spec.Handle);
			}
		}
	}
}

void UFPSCombatBaseGameplayAbility::OnPawnAvatarSet()
{
}

AController* UFPSCombatBaseGameplayAbility::GetController()
{
	check(CurrentActorInfo);
	
	if (AController* Controller = CurrentActorInfo->PlayerController.Get())
	{
		return Controller;
	}

	AActor* OwnerActor = CurrentActorInfo->OwnerActor.Get();
	while (OwnerActor)
	{
		if (AController* Controller = Cast<AController>(OwnerActor))
		{
			return Controller;
		}

		if (const APawn* Pawn = Cast<APawn>(OwnerActor))
		{
			return Pawn->GetController();
		}
		
		OwnerActor = OwnerActor->GetOwner();
	}
	return nullptr;
}

AController* UFPSCombatBaseGameplayAbility::GetController()
{
	check(CurrentActorInfo);
	
	if (AController* Controller = CurrentActorInfo->PlayerController.Get())
	{
		return Controller;
	}

	AActor* OwnerActor = CurrentActorInfo->OwnerActor.Get();
	while (OwnerActor)
	{
		if (AController* Controller = Cast<AController>(OwnerActor))
		{
			return Controller;
		}

		if (const APawn* Pawn = Cast<APawn>(OwnerActor))
		{
			return Pawn->GetController();
		}
		
		OwnerActor = OwnerActor->GetOwner();
	}
	return nullptr;
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
