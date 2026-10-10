// FPS Combat project


#include "Animation/FPSCombatAnimInstance.h"

#include "AbilitySystemGlobals.h"
#include "Characters/FPSCombatCharacter.h"
#include "Characters/FPSCombatMovementComponent.h"

UFPSCombatAnimInstance::UFPSCombatAnimInstance(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

void UFPSCombatAnimInstance::InitializeWithAbilitySystem(UAbilitySystemComponent* ASC)
{
	check(ASC);

	// Maps the ASC gameplay tags to bool properties on the animation instance.
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
	
	UFPSCombatMovementComponent* MovementComp = Cast<UFPSCombatMovementComponent>(Character->GetCombatMovementComponent());
	if (!MovementComp) return;
	// Expose the cached ground distance to the animation graph.
	const FPSCombatGroundInfo& GroundInfo = MovementComp->GetGroundInfo();
	GroundDistance = GroundInfo.GroundDistance;
}
