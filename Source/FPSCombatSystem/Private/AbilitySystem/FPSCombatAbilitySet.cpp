// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/FPSCombatAbilitySet.h"

void FCombatAbilitySet_GrantedHandles::AddAbilities(const FGameplayAbilitySpecHandle& Handle)
{
	if (Handle.IsValid())
	{
		AbilitySpecHandles.Add(Handle);
	}
}

void FCombatAbilitySet_GrantedHandles::AddGameplayEffects(const FActiveGameplayEffectHandle& Handle)
{
	if (Handle.IsValid())
	{
		EffectSpecHandles.Add(Handle);
	}
}

void FCombatAbilitySet_GrantedHandles::AddAttributeSets(UAttributeSet* Set)
{
	AttributeSets.Add(Set);
}

void FCombatAbilitySet_GrantedHandles::ClearAbilitySystem(UFPSCombatAbilitySystemComponent* ASC)
{
	if (!ASC || !ASC->IsOwnerActorAuthoritative()) return;
	
	for (const FGameplayAbilitySpecHandle& Handle : AbilitySpecHandles)
	{
		if (Handle.IsValid())
		{
			ASC->ClearAbility(Handle);
		}
	}

	for (const FActiveGameplayEffectHandle& Handle : EffectSpecHandles)
	{
		if (Handle.IsValid())
		{
			ASC->RemoveActiveGameplayEffect(Handle);
		}
	}
	
	for (UAttributeSet* Set : AttributeSets)
	{
		ASC->RemoveSpawnedAttribute(Set);
	}
	
	AbilitySpecHandles.Reset();
	EffectSpecHandles.Reset();
	AttributeSets.Reset();
}

void UFPSCombatAbilitySet::GiveAbility(UFPSCombatAbilitySystemComponent* ASC,
	FCombatAbilitySet_GrantedHandles* GrantedHandles, UObject* SpecObject) const
{
	if (!ASC->IsOwnerActorAuthoritative()) return;


	for (int32 IndexAbility = 0; IndexAbility< GrantedGameplayAbilities.Num(); IndexAbility++)
	{
		const FCombatAbilitySet_Abilities& AbilityToGrant = GrantedGameplayAbilities[IndexAbility];

		if (!IsValid(AbilityToGrant.GameplayAbility))
		{
			UE_LOG(LogTemp, Warning, TEXT("An ability is not add"));
			continue;
		}
		
		FGameplayAbilitySpec Spec(AbilityToGrant.GameplayAbility, AbilityToGrant.AbilityLevel);
		Spec.SourceObject = SpecObject;
		Spec.GetDynamicSpecSourceTags().AddTag(AbilityToGrant.InputTag);

		const FGameplayAbilitySpecHandle SpecHandle = ASC->GiveAbility(Spec);

		if (GrantedHandles)
		{
			GrantedHandles->AddAbilities(SpecHandle);
		}
	}

	for (int32 IndexEffect = 0; IndexEffect < GrantedGameplayEffects.Num(); IndexEffect++)
	{
		const FCombatAbilitySet_Effects& EffectToGrant = GrantedGameplayEffects[IndexEffect];
		if (!IsValid(EffectToGrant.GameplayEffect))
		{
			UE_LOG(LogTemp, Warning, TEXT("An effect is not add"));
			continue;
		}

		const UGameplayEffect* GameplayEffect = EffectToGrant.GameplayEffect.GetDefaultObject();
		const FActiveGameplayEffectHandle Handle = ASC->ApplyGameplayEffectToSelf(GameplayEffect, EffectToGrant.EffectLevel, ASC->MakeEffectContext());


		if (GrantedHandles)
		{
			GrantedHandles->AddGameplayEffects(Handle);
		}
	}

	for (int32 IndexAttribute = 0; IndexAttribute < GrantedGameplayAttributes.Num(); IndexAttribute++)
	{
		const FCombatAbilitySet_AttributeSets& AttributeToGrant = GrantedGameplayAttributes[IndexAttribute];
		if (!IsValid(AttributeToGrant.AttributeSet))
		{
			UE_LOG(LogTemp, Warning, TEXT("An attribute set is not add"));
			continue;
		}

		UAttributeSet* AttributeSet = NewObject<UAttributeSet>(ASC->GetOwner(),AttributeToGrant.AttributeSet);
		ASC->AddSpawnedAttribute(AttributeSet);

		if (GrantedHandles)
		{
			GrantedHandles->AddAttributeSets(AttributeSet);
		}
	}
}
