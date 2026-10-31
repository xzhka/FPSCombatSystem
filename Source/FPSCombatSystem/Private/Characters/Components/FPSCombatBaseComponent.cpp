// FPS Combat project


#include "Characters/Components/FPSCombatBaseComponent.h"

#include "FPSCombatSystem/FPSCombatSystem.h"


void UFPSCombatBaseComponent::InitializeWithAbilitySystem(UFPSCombatAbilitySystemComponent* ASC)
{
	AActor* Actor = GetOwner();

	check(Actor);

	AbilitySystem = ASC;

	if (!AbilitySystem)
	{
		UE_LOG(LogFPSCombat, Warning, TEXT("%s: owner has no AbilitySystemComponent."), *GetNameSafe(GetOwner()));
		return;
	}

	AttributeSet = AbilitySystem->GetSet<UFPSCombatAttributeSet>();

	if (!AttributeSet)
	{
		UE_LOG(LogFPSCombat, Warning, TEXT("%s: owner has no AttributeSet."), *GetNameSafe(GetOwner()));
		return;
	}

	// Derived component classes binds to the attribute set
	// delegate here.
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
