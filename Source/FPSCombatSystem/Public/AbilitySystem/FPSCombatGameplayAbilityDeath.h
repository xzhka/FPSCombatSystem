// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "FPSCombatGameplayAbilityDeath.generated.h"

/**
 * 
 */
UCLASS()
class FPSCOMBATSYSTEM_API UFPSCombatGameplayAbilityDeath : public UGameplayAbility
{
	GENERATED_BODY()
	
	UFPSCombatGameplayAbilityDeath();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:

	UFUNCTION(BlueprintCallable, Category = "DeathEvent")
	void StartDeath();

	UFUNCTION(BlueprintCallable, Category = "DeathEvent")
	void FinishDeath();
	
	
};
