// FPS Combat project

#pragma once

#include "CoreMinimal.h"
#include "FPSCombatQuickBarComponent.h"
#include "AbilitySystem/Abilities/FPSCombatBaseGameplayAbility.h"
#include "FPSCombatGameplayAbilityQuickBar.generated.h"

/** UFPSCombatGameplayAbilityQuickBar
 *
 *  Ability associated with the quick bar component
 */
UCLASS()
class FPSCOMBATSYSTEM_API UFPSCombatGameplayAbilityQuickBar : public UFPSCombatBaseGameplayAbility
{
	GENERATED_BODY()

	UFPSCombatGameplayAbilityQuickBar();
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	void ActionTagSelector(UFPSCombatQuickBarComponent* QuickBarComponent);
};
