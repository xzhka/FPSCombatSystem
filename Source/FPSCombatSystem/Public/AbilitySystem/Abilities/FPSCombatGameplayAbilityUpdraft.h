// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FPSCombatBaseGameplayAbility.h"
#include "Abilities/GameplayAbility.h"
#include "FPSCombatGameplayAbilityUpdraft.generated.h"

/**
 * 
 */
UCLASS()
class FPSCOMBATSYSTEM_API UFPSCombatGameplayAbilityUpdraft : public UFPSCombatBaseGameplayAbility
{
	GENERATED_BODY()

public:
	UFPSCombatGameplayAbilityUpdraft();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	
	void Updraft();

	UPROPERTY()
	FActiveGameplayEffectHandle AirborneEffectHandle;

	UFUNCTION()
	void HandleUpdraftSourceChanged(const FGameplayEffectRemovalInfo& RemovalInfo);

	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> AbilityTagGrantEffect;
	
	UPROPERTY(EditDefaultsOnly, Category = "Tags")
	FGameplayTag CueTag;
	
	UPROPERTY(EditDefaultsOnly, Category = "Params")
	float Distance = 0.f;
};
