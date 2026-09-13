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
	/* Functions */
	
	/* Ability overrides */
    virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
       const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr,
       FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
    virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
       const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;
	/* End ability overrides */
    virtual EFPSCombatAbilityActivationPolicy GetActivationPolicy(const FGameplayAbilitySpec& Spec) const override;
	
    UFUNCTION(BlueprintCallable, Category = "Ability")
    UFPSCombatRangedWeaponInstance* GetWeaponInstance() const;
    
protected:
	/* Functions */

	/* Ability overrides */
    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
       const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
    virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
       const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	/* End ability overrides */

	
    virtual void NotifyInputReleased(const FGameplayAbilitySpec& Spec) override;

    virtual ECollisionChannel DetermineTraceChannel() const;

	UFUNCTION(BlueprintImplementableEvent, Category = "Weapon", meta = (DisplayName = "On Fire Shot"))
	void K2_OnShotFire();

	/* Variables */
	
    UPROPERTY(EditDefaultsOnly, Category = "Validation", meta = (ClampMin = "0.0", Units = "cm"))
    float HitValidationRangeSlack = 60.f;

    UPROPERTY(EditDefaultsOnly, Category = "Validation", meta = (ClampMin = "0.0", Units = "cm"))
    float HitValidationTolerance = 20.f;

    UPROPERTY(EditDefaultsOnly, Category = "Tags")
    FGameplayTag OnOutOfAmmoTag;

	UPROPERTY(EditDefaultsOnly, Category = "Tags")
	FGameplayTag ImpactTag;
	
private:

	struct FFPSInFlightShot
	{
		FGameplayAbilitySpecHandle SpecHandle;
		FPredictionKey PredictionKey;
		FDelegateHandle DataReadyHandle;
		FDelegateHandle DataCancelledHandle;
	};

	/* Functions */
	
    void FireShot();

    void ApplyDamageForShot(const FGameplayAbilityTargetDataHandle& DataHandle) const;
    
    void StartRangedWeaponTargeting(const FFPSCombatShotContext& Context);
    void PerformLocalTargeting(const FFPSCombatShotContext& Context, OUT TArray<FHitResult>& OutHits);

	void BindShotConfirmation(FGameplayAbilitySpecHandle Handle, FPredictionKey PredictionKey);
	void OnShotTargetDataReady(const FGameplayAbilityTargetDataHandle& DataHandle, FGameplayTag ApplicationTag, FPredictionKey ShotKey);
	void OnShotTargetDataCancelled(FPredictionKey ShotKey);
	
    bool IsHitResultValid(const FHitResult& HitResult) const;

	/* Variables */
	
    bool bHasTargetDataSent = false;

	TArray<FFPSInFlightShot> InFlightShots;
};
