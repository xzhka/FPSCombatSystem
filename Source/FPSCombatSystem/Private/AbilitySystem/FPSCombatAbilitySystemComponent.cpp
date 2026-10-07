// FPS Combat project


#include "AbilitySystem/FPSCombatAbilitySystemComponent.h"

#include "AbilitySystem/Abilities/FPSCombatBaseGameplayAbility.h"
#include "Animation/FPSCombatAnimInstance.h"

UFPSCombatAbilitySystemComponent::UFPSCombatAbilitySystemComponent()
{
	PressedAbilitySpecHandles.Reset();
	ReleasedAbilitySpecHandles.Reset();
	HeldAbilitySpecHandles.Reset();
}

void UFPSCombatAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	if (InputTag.IsValid())
	{
		for (FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
		{
			if (Spec.Ability && (Spec.GetDynamicSpecSourceTags().HasTagExact(InputTag)))
			{
				PressedAbilitySpecHandles.AddUnique(Spec.Handle);
				HeldAbilitySpecHandles.AddUnique(Spec.Handle);
			}
		}
	}
}

void UFPSCombatAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (InputTag.IsValid())
	{
		for (FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
		{
			if (Spec.Ability && (Spec.GetDynamicSpecSourceTags().HasTagExact(InputTag)))
			{
				ReleasedAbilitySpecHandles.AddUnique(Spec.Handle);
				HeldAbilitySpecHandles.Remove(Spec.Handle);

				// if (UFPSCombatBaseGameplayAbility* CombatAbility = Cast<UFPSCombatBaseGameplayAbility>(Spec.Ability))
				// {
				// 	CombatAbility->NotifyInputReleased(Spec);
				// }
			}
		}
	}
}

void UFPSCombatAbilitySystemComponent::InitializeDefaultAttributes()
{
	FGameplayEffectContextHandle Context = MakeEffectContext();
	Context.AddSourceObject(GetOwnerActor());
	for (const TSubclassOf<UGameplayEffect>& GameplayEffectSpec : DefaultGameplayEffect)
	{
		const FGameplayEffectSpecHandle& Spec = MakeOutgoingSpec(GameplayEffectSpec, 1.0f, Context);
		if (Spec.IsValid())
		{
			ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
		}
	}
}

void UFPSCombatAbilitySystemComponent::ProcessAbilityInput(float DeltaTime, bool bGamePaused)
{
	static TArray<FGameplayAbilitySpecHandle> AbilitiesToActivate;
	AbilitiesToActivate.Reset();

	for (const FGameplayAbilitySpecHandle& SpecHandle : HeldAbilitySpecHandles)
	{
		if (const FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability && !AbilitySpec->IsActive())
			{
				const UFPSCombatBaseGameplayAbility* AbilityCDO = Cast<UFPSCombatBaseGameplayAbility>(AbilitySpec->Ability);
				if (AbilityCDO && AbilityCDO->GetActivationPolicy(*AbilitySpec) == EFPSCombatAbilityActivationPolicy::WhileInputActive)
				{
					AbilitiesToActivate.AddUnique(SpecHandle);
				}
			}
		}
	}

	for (const FGameplayAbilitySpecHandle& SpecHandle : PressedAbilitySpecHandles)
	{
		if ( FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability)
			{
				AbilitySpec->InputPressed = true;

				if (AbilitySpec->IsActive())
				{
					AbilitySpecInputPressed(*AbilitySpec);
				}
				else
				{
					const UFPSCombatBaseGameplayAbility* AbilityCDO = Cast<UFPSCombatBaseGameplayAbility>(AbilitySpec->Ability);
					if (AbilityCDO && AbilityCDO->GetActivationPolicy(*AbilitySpec) == EFPSCombatAbilityActivationPolicy::OnInputTriggered)
					{
						AbilitiesToActivate.AddUnique(SpecHandle);
					}
				}
			}
		}
	}
	

	for (const FGameplayAbilitySpecHandle& SpecHandle : AbilitiesToActivate)
	{
		TryActivateAbility(SpecHandle);
	}

	for (const FGameplayAbilitySpecHandle& SpecHandle : ReleasedAbilitySpecHandles)
	{
		if ( FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (Spec->Ability)
			{
				Spec->InputPressed = false;
				if (Spec->IsActive())
				{
					AbilitySpecInputReleased(*Spec);
				}
			}
		}
	}

	PressedAbilitySpecHandles.Reset();
	ReleasedAbilitySpecHandles.Reset();
}

void UFPSCombatAbilitySystemComponent::ClearAbilityInput()
{
	PressedAbilitySpecHandles.Reset();
	ReleasedAbilitySpecHandles.Reset();
	HeldAbilitySpecHandles.Reset();
}

void UFPSCombatAbilitySystemComponent::ApplyAbilityBlockAndCancelTags(const FGameplayTagContainer& AbilityTags,
	UGameplayAbility* RequestingAbility, bool bEnableBlockTags, const FGameplayTagContainer& BlockTags,
	bool bExecuteCancelTags, const FGameplayTagContainer& CancelTags)
{
	FGameplayTagContainer MergedBlock = BlockTags;
	FGameplayTagContainer MergedCancel = CancelTags;

	if (RelationshipMapping)
	{
		RelationshipMapping->GetAbilityTagsToBlockAndCancel(AbilityTags, &MergedBlock, &MergedCancel);
	}
	
	Super::ApplyAbilityBlockAndCancelTags(AbilityTags, RequestingAbility, bEnableBlockTags, MergedBlock,
	                                      bExecuteCancelTags, MergedCancel);
}

void UFPSCombatAbilitySystemComponent::TryActivateAbilityOnSpawn()
{
	for (const FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
	{
		if (const UFPSCombatBaseGameplayAbility* AbilityCDO = Cast<UFPSCombatBaseGameplayAbility>(Spec.Ability))
		{
			AbilityCDO->TryActivateAbilityOnSpawn(AbilityActorInfo.Get(), Spec);
		}
	}
}

void UFPSCombatAbilitySystemComponent::InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
	FGameplayAbilityActorInfo* ActorInfo = AbilityActorInfo.Get();
	check(InOwnerActor);
	check(ActorInfo);

	const bool bIsNewPawnAvatar = Cast<APawn>(InAvatarActor) && (InAvatarActor != ActorInfo->AvatarActor);
	
	Super::InitAbilityActorInfo(InOwnerActor, InAvatarActor);

	if (bIsNewPawnAvatar)
	{
		for (const FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
		{
			TArray<UGameplayAbility*> Instances = Spec.GetAbilityInstances();
			for (UGameplayAbility* AbilityInstances : Instances)
			{
				UFPSCombatBaseGameplayAbility* FPSCombatAbilityInstances = Cast<UFPSCombatBaseGameplayAbility>(AbilityInstances);
				if (FPSCombatAbilityInstances)
				{
					FPSCombatAbilityInstances->OnPawnAvatarSet();
				}
			}
		}

		if (UFPSCombatAnimInstance* AnimInstance = Cast<UFPSCombatAnimInstance>(ActorInfo->GetAnimInstance()))
		{
			AnimInstance->InitializeWithAbilitySystem(this);
		}
		

		TryActivateAbilityOnSpawn();
	}
}

void UFPSCombatAbilitySystemComponent::AbilitySpecInputPressed(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputPressed(Spec);

	if (Spec.IsActive())
	{
		if (const UGameplayAbility* PrimaryAbility = Spec.GetPrimaryInstance())
		{
			FPredictionKey PredictionKey = PrimaryAbility->GetCurrentActivationInfo().GetActivationPredictionKey();
			InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, Spec.Handle, PredictionKey);
		}
	}
}

void UFPSCombatAbilitySystemComponent::AbilitySpecInputReleased(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputReleased(Spec);

	if (Spec.IsActive())
	{
		if (const UGameplayAbility* PrimaryAbility = Spec.GetPrimaryInstance())
		{
			FPredictionKey PredictionKey = PrimaryAbility->GetCurrentActivationInfo().GetActivationPredictionKey();
			InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, Spec.Handle, PredictionKey);
		}
	}
}
