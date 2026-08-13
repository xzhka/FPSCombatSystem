// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Components/FPSCombatStaminaComponent.h"

#include "FPSCombatSystem/FPSCombatGameplayTags.h"


UFPSCombatStaminaComponent::UFPSCombatStaminaComponent()
{
	AbilitySystem = nullptr;
	AttributeSet = nullptr;
}

float UFPSCombatStaminaComponent::GetStamina() const
{
	return (AttributeSet ? AttributeSet->GetStamina() : 0.f);
}

float UFPSCombatStaminaComponent::GetMaxStamina() const
{
	return (AttributeSet ? AttributeSet->GetMaxStamina() : 0.f);
}

float UFPSCombatStaminaComponent::GetMergedStamina() const
{
	if (AttributeSet)
	{
		const float OldStamina = AttributeSet->GetStamina();
		const float MaxStamina = AttributeSet->GetMaxStamina();

		return (MaxStamina > 0) ? (OldStamina / MaxStamina) : 0.f;
	}
	return 0.0f;
}

void UFPSCombatStaminaComponent::BindAttributeDelegate()
{

	AttributeSet->OnStaminaChanged.AddUObject(this, &ThisClass::HandleStaminaChanged);
	AttributeSet->OnMaxStaminaChanged.AddUObject(this, &ThisClass::HandleMaxStaminaChanged);
	AttributeSet->OnStaminaDepleted.AddUObject(this, &ThisClass::HandleStaminaDepletedChanged);
	AttributeSet->OnStaminaRestored.AddUObject(this, &ThisClass::HandleStaminaRestoredChanged);

	AbilitySystem->SetNumericAttributeBase(UFPSCombatAttributeSet::GetStaminaAttribute(), GetMaxStamina());
	

	OnStaminaChanged.Broadcast(this, AttributeSet->GetStamina(), AttributeSet->GetMaxStamina());
	OnMaxStaminaChanged.Broadcast(this, AttributeSet->GetStamina(), AttributeSet->GetMaxStamina());
	OnPercentChanged.Broadcast(GetMergedStamina());
}

void UFPSCombatStaminaComponent::UnBindAttributeDelegate()
{
	AttributeSet->OnStaminaChanged.RemoveAll(this);
	AttributeSet->OnMaxStaminaChanged.RemoveAll(this);
	AttributeSet->OnStaminaDepleted.RemoveAll(this);
	AttributeSet->OnStaminaRestored.RemoveAll(this);
}

void UFPSCombatStaminaComponent::HandleStaminaChanged(float OldValue, float NewValue)
{
	OnStaminaChanged.Broadcast(this, OldValue, NewValue);
	OnPercentChanged.Broadcast(GetMergedStamina());
}

void UFPSCombatStaminaComponent::HandleMaxStaminaChanged(float OldValue, float NewValue)
{
	OnMaxStaminaChanged.Broadcast(this, OldValue, NewValue);
	OnPercentChanged.Broadcast(GetMergedStamina());
}

void UFPSCombatStaminaComponent::HandleStaminaDepletedChanged()
{
	if (AbilitySystem)
	{
		AbilitySystem->SetLooseGameplayTagCount(FPSCombatGameplayTags::State_Stamina_OutOfStamina, 1);
		
		FGameplayTagContainer TagContainer;
		TagContainer.AddTag(FPSCombatGameplayTags::Ability_RequiresStamina);
		AbilitySystem->CancelAbilities(&TagContainer);
	}

	OnStaminaDepleted.Broadcast(GetOwner());
}

void UFPSCombatStaminaComponent::HandleStaminaRestoredChanged()
{
	if (AbilitySystem)
	{
		AbilitySystem->SetLooseGameplayTagCount(FPSCombatGameplayTags::State_Stamina_OutOfStamina, 0);
	}
	
	OnStaminaRestored.Broadcast(GetOwner());
}