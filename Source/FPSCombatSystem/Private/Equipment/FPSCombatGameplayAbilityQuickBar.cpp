// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipment/FPSCombatGameplayAbilityQuickBar.h"

#include "Equipment/FPSCombatQuickBarComponent.h"
#include "FPSCombatSystem/FPSCombatGameplayTags.h"

void UFPSCombatGameplayAbilityQuickBar::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                        const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                        const FGameplayEventData* TriggerEventData)
{
	check(ActorInfo);

	AController* Controller = GetController();
	
	UFPSCombatQuickBarComponent* QuickBarComponent = Controller->GetComponentByClass<UFPSCombatQuickBarComponent>();
	
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (IsValid(QuickBarComponent))
	{
		ActionTagSelector(QuickBarComponent);
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UFPSCombatGameplayAbilityQuickBar::ActionTagSelector(UFPSCombatQuickBarComponent* QuickBarComponent)
{
	const FGameplayAbilitySpec* Spec = GetCurrentAbilitySpec();
	
	if (Spec->GetDynamicSpecSourceTags().HasTagExact(FPSCombatGameplayTags::Input_QuickBar_Next))
	{
		QuickBarComponent->CycleSlotForward();
	}
	else if (Spec->GetDynamicSpecSourceTags().HasTagExact(FPSCombatGameplayTags::Input_QuickBar_Previous))
	{
		QuickBarComponent->CycleSlotBackward();
	}
	else if (Spec->GetDynamicSpecSourceTags().HasTagExact(FPSCombatGameplayTags::Input_QuickBar_Select1))
	{
		QuickBarComponent->SetActiveSlot(0);
	}
	else if (Spec->GetDynamicSpecSourceTags().HasTagExact(FPSCombatGameplayTags::Input_QuickBar_Select2))
	{
		QuickBarComponent->SetActiveSlot(1);
	}
	else if (Spec->GetDynamicSpecSourceTags().HasTagExact(FPSCombatGameplayTags::Input_QuickBar_Select3))
	{
		QuickBarComponent->SetActiveSlot(2);
	}
}