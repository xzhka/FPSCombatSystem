// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/FPSCombatAbilitySystemComponent.h"

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
		for (const FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
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
		for (const FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
		{
			if (Spec.Ability && (Spec.GetDynamicSpecSourceTags().HasTagExact(InputTag)))
			{
				ReleasedAbilitySpecHandles.AddUnique(Spec.Handle);
				HeldAbilitySpecHandles.Remove(Spec.Handle);
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
				AbilitiesToActivate.AddUnique(SpecHandle);
			}
		}
	}

	for (const FGameplayAbilitySpecHandle& SpecHandle : PressedAbilitySpecHandles)
	{
		if ( FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (Spec->Ability)
			{
				Spec->InputPressed = true;

				if (Spec->IsActive())
				{
					AbilitySpecInputPressed(*Spec);
				}
				else
				{
					AbilitiesToActivate.AddUnique(SpecHandle);
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
	UE_LOG(LogTemp, Display, TEXT("ApplyAbilityBlockAndCancelTags"));
	FGameplayTagContainer MergedBlock = BlockTags;
	FGameplayTagContainer MergedCancel = CancelTags;

	if (RelationshipMapping)
	{
		RelationshipMapping->GetAbilityTagsToBlockAndCancel(AbilityTags, &MergedBlock, &MergedCancel);
	}
	
	Super::ApplyAbilityBlockAndCancelTags(AbilityTags, RequestingAbility, bEnableBlockTags, MergedBlock,
	                                      bExecuteCancelTags, MergedCancel);
}
