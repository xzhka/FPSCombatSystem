// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Components/FPSCombatBaseComponent.h"


void UFPSCombatBaseComponent::InitializeWithAbilitySystem(UFPSCombatAbilitySystemComponent* ASC)
{
	AActor* Actor = GetOwner();

	check(Actor);

	AbilitySystem = ASC;

	if (!AbilitySystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("AbilitySystem unauthorize in component"));
		return;
	}

	AttributeSet = AbilitySystem->GetSet<UFPSCombatAttributeSet>();

	if (!AttributeSet)
	{
		UE_LOG(LogTemp, Warning, TEXT("AttributeSet unauthorize in component"));
		return;
	}

	BindAttributeDelegate();
}

void UFPSCombatBaseComponent::UninitializeFromAbilitySystem()
{
	if (AttributeSet)
	{
		UnBindAttributeDelegate();
	}

	AttributeSet = nullptr;
	AbilitySystem = nullptr;
}
