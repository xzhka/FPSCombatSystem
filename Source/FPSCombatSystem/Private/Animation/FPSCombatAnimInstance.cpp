// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/FPSCombatAnimInstance.h"

#include "AbilitySystemGlobals.h"
#include "Characters/FPSCombatCharacter.h"
#include "Characters/FPSCombatMovementComp.h"

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

void UFPSCombatAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	const AFPSCombatCharacter* Character = Cast<AFPSCombatCharacter>(GetOwningActor());

	if (!Character) return;
	
	UFPSCombatMovementComp* MovementComp = CastChecked<UFPSCombatMovementComp>(Character->GetCombatMovementComponent());
	bIsAirborne = MovementComp->GetCurrentState() == EFPSCombatMoveState::Airborne;
	if (bIsAirborne)
	{
		const FPSCombatGroundInfo& GroundInfo = MovementComp->GetGroundInfo();
		GroundDistance = GroundInfo.GroundDistance;
	}
}
