// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FPSCombatRangedWeaponInstance.h"
#include "AbilitySystem/Abilities/FPSCombatBaseGameplayAbility.h"
#include "FPSCombatGameplayAbilityRangedW.generated.h"

/**
 * 
 */
UCLASS()
class FPSCOMBATSYSTEM_API UFPSCombatGameplayAbilityRangedW : public UFPSCombatBaseGameplayAbility
{
	GENERATED_BODY()

	UFPSCombatGameplayAbilityRangedW();
	
public:
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr,
		FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo) override;

	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo) override;
	
	UFUNCTION(BlueprintCallable, Category = "Ability")
	UFPSCombatRangedWeaponInstance* GetWeaponInstance() const;
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	void StartRangedWeaponTargeting();
	
	void PerformLocalTargeting(OUT TArray<FHitResult>& OutHits);

	void OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& DataHandle, FGameplayTag ApplicationTag);

	virtual ECollisionChannel DetermineTraceChannel() const;
	
	bool IsHitResultValid(const FHitResult& HitResult) const;
	
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditDefaultsOnly, Category = "Validation", meta = (ClampMin = "0.0", Units = "cm"))
	float HitValidationRangeSlack = 60.f;

	UPROPERTY(EditDefaultsOnly, Category = "Validation", meta = (ClampMin = "0.0", Units = "cm"))
	float HitValidationTolerance = 20.f;

	UFUNCTION()
	void OnInputReleased(float TimeHeld);

	void HandleFireInput();

	UPROPERTY(EditDefaultsOnly, Category = "Tags")
	FGameplayTag OnOutOfAmmo;

	void HandleNextShot();

	UFUNCTION()
	void OnShotDelayFinished();
	
	UPROPERTY(Transient)
	TObjectPtr<class UAbilityTask_WaitInputRelease> WaitInputReleaseTask;

	UPROPERTY(Transient)
	TObjectPtr<class UAbilityTask_WaitDelay> WaitDelayTask;
	
	bool bWantsToFire = false;
	
};
