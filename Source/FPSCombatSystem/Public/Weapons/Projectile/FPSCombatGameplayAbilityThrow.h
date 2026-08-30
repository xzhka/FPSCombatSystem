// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FPSCombatThrowableInstance.h"
#include "AbilitySystem/Abilities/FPSCombatBaseGameplayAbility.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "FPSCombatGameplayAbilityThrow.generated.h"

/**
 * 
 */
UCLASS()
class FPSCOMBATSYSTEM_API UFPSCombatGameplayAbilityThrow : public UFPSCombatBaseGameplayAbility
{
	GENERATED_BODY()

public:
	UFPSCombatGameplayAbilityThrow();
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo) override;
	
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
	UFUNCTION()
	void OnReleaseNotify(float TimeHandle);

	void SpawnAndLaunchProjectile();

	void OnReleaseNotifyTimeout();

	/* Variables */
	UPROPERTY()
	TObjectPtr<class UAbilityTask_WaitInputRelease> WaitInputRelease;
	
	
	UPROPERTY(EditDefaultsOnly, Category = "Throw")
	FGameplayTag ReleaseEventTag;
	
	UPROPERTY(EditDefaultsOnly, Category = "Throw")
	float SpawnOffset = 80.f;

	UPROPERTY()
	FTimerHandle MaxHoldHandle;

	UPROPERTY(EditDefaultsOnly, Category = "Time")
	float MaxHoldTime = 0.f;
	
private:
	UPROPERTY()
	TObjectPtr<UFPSCombatThrowableInstance> ThrowInstance = nullptr;
};
