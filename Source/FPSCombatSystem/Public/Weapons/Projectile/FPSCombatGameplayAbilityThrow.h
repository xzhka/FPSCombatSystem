// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FPSCombatThrowableInstance.h"
#include "AbilitySystem/Abilities/FPSCombatBaseGameplayAbility.h"
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
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	UFUNCTION()
	void OnReleaseNotify(FGameplayEventData Payload);

	void SpawnAndLaunchProjectile();

	UPROPERTY(EditDefaultsOnly, Category = "Throw")
	FGameplayTag ReleaseEventTag;
	
	UPROPERTY(EditDefaultsOnly, Category = "Throw")
	float SpawnOffset = 80.f;
	
	
private:
	UPROPERTY()
	TObjectPtr<UFPSCombatThrowableInstance> ThrowInstance = nullptr;
};
