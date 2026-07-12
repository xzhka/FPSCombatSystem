// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSCombatSystem/Public/GameModes/FPSCombatPlayerState.h"

AFPSCombatPlayerState::AFPSCombatPlayerState()
{
	ASC = CreateDefaultSubobject<UFPSCombatAbilitySystemComponent>(TEXT("AbilitySystemComponent"));

	ASC->SetIsReplicated(true);
	SetNetUpdateFrequency(100.f);
	ASC->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	
	AttributeSet = CreateDefaultSubobject<UFPSCombatAttributeSet>(TEXT("AttributeSet"));
}

UAbilitySystemComponent* AFPSCombatPlayerState::GetAbilitySystemComponent() const
{
	return ASC;
}

UAttributeSet* AFPSCombatPlayerState::GetAttributeSet() const
{
	return AttributeSet;
}
