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
	
	UFPSCombatGameplayAbilityThrow();
	
protected:
	/* Functions */
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	virtual void CommitExecute(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo) override;
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr,
		FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	
	UFUNCTION(BlueprintCallable)
	void SpawnAndLaunchProjectile();

	void OnReleaseNotifyTimeout();

	UFUNCTION()
	void HandleInputReleased(float TimeHandle);
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Task", meta = (DisplayName = "On Complete"))
	void K2_OnThrowSetupComplete();

	UFUNCTION(BlueprintImplementableEvent, Category = "Task", meta = (DisplayName = "On Released"))
	void K2_OnThrowReleased(float TimeHandle);

	UFUNCTION(BlueprintCallable, Category = "Throw", DisplayName = "Apply Throw Cooldown")
	void BP_ApplyThrowCooldown() { ApplyCooldown(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo); }

	UFUNCTION(BlueprintCallable, Category = "Throw")
	UFPSCombatThrowableDefinition* GetThrowableDefinition() const { return ThrowInstance ? ThrowInstance->GetThrowableDefinition() : nullptr; }

	UFUNCTION(BlueprintCallable, Category = "Throw", DisplayName = "Get Hold Montage")
	UAnimMontage* BP_GetHoldMontage() const;

	UFUNCTION(BlueprintCallable, Category = "Throw", DisplayName = "Get Throw Montage")
	UAnimMontage* BP_GetThrowMontage() const;
	
	/* Variables */
	UPROPERTY()
	TObjectPtr<class UAbilityTask_WaitInputRelease> WaitInputRelease;
	
	
	UPROPERTY(EditDefaultsOnly, Category = "Throw")
	FGameplayTag ReleaseEventTag;

	UPROPERTY()
	FTimerHandle MaxHoldHandle;

	UPROPERTY(EditDefaultsOnly, Category = "Time")
	float MaxHoldTime = 0.f;
	
private:
	UPROPERTY()
	TObjectPtr<UFPSCombatThrowableInstance> ThrowInstance = nullptr;

	UPROPERTY()
	TObjectPtr<UFPSCombatEquipmentInstance> CachedItemInstance;
};
