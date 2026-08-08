// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/FPSCombatAnimInstance.h"

#include "AbilitySystemGlobals.h"
#include "Characters/FPSCombatCharacter.h"

UFPSCombatAnimInstance::UFPSCombatAnimInstance(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

void UFPSCombatAnimInstance::InitializeWithAbilitySystem(UAbilitySystemComponent* ASC)
{
	check(ASC);

	GameplayTagPropertyMap.Initialize(this, ASC);
}

void UFPSCombatAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if (AActor* OwningActor = GetOwningActor())
	{
		if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwningActor))
		{
			InitializeWithAbilitySystem(ASC);
		}
	}
}
