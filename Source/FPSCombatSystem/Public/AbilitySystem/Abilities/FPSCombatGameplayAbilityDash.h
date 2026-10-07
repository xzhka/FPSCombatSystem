// FPS Combat project

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "AbilitySystem/Abilities/FPSCombatBaseGameplayAbility.h"
#include "FPSCombatGameplayAbilityDash.generated.h"

/** UFPSCombatGameplayAbilityDash
 *
 * Dash representing ability
 */
UCLASS()
class FPSCOMBATSYSTEM_API UFPSCombatGameplayAbilityDash : public UFPSCombatBaseGameplayAbility
{
	GENERATED_BODY()

public:
	UFPSCombatGameplayAbilityDash();

	virtual void CommitExecute(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo) override;
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
	
	void Dash();
	
	UPROPERTY(EditDefaultsOnly, Category = "Duration")
	float Duration;

	UPROPERTY(EditDefaultsOnly, Category = "Strength")
	float Strength;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TSubclassOf<UGameplayEffect> DashGrantedEffectsClass;

	UPROPERTY()
	TArray<FActiveGameplayEffectHandle> DashGrantedEffectHandle;
	
};
