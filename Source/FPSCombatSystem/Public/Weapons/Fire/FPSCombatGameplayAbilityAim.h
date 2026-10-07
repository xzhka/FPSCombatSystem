// FPS Combat project

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/FPSCombatBaseGameplayAbility.h"
#include "FPSCombatGameplayAbilityAim.generated.h"

/** UFPSCombatGameplayAbilityAim
 *
 *	Ability associated with the aim 
 */
UCLASS()
class FPSCOMBATSYSTEM_API UFPSCombatGameplayAbilityAim : public UFPSCombatBaseGameplayAbility
{
	GENERATED_BODY()

public:
	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo) override;
};
