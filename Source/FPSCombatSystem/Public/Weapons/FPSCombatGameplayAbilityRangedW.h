// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FPSCombatRangedWeaponInstance.h"
#include "AbilitySystem/Abilities/FPSCombatBaseGameplayAbility.h"
#include "Fire/FPSCombatShotTracker.h"
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
    virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
       const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;

    virtual EFPSCombatAbilityActivationPolicy GetActivationPolicy(const FGameplayAbilitySpec& Spec) const override;
    
    UFUNCTION(BlueprintCallable, Category = "Ability")
    UFPSCombatRangedWeaponInstance* GetWeaponInstance() const;
    
protected:
    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
       const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
    virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
       const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

    virtual void NotifyInputReleased(const FGameplayAbilitySpec& Spec) override;

    virtual ECollisionChannel DetermineTraceChannel() const;
    

    UPROPERTY(EditDefaultsOnly, Category = "Validation", meta = (ClampMin = "0.0", Units = "cm"))
    float HitValidationRangeSlack = 60.f;

    UPROPERTY(EditDefaultsOnly, Category = "Validation", meta = (ClampMin = "0.0", Units = "cm"))
    float HitValidationTolerance = 20.f;

    UPROPERTY(EditDefaultsOnly, Category = "Tags")
    FGameplayTag OnOutOfAmmo;

private:
    
    void FireShot();

    void ApplyDamageForShot(const FGameplayAbilityTargetDataHandle& DataHandle) const;
    
    void StartRangedWeaponTargeting();
    void PerformLocalTargeting(OUT TArray<FHitResult>& OutHits);
    
    void OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& DataHandle, FGameplayTag ApplicationTag);
    void OnTargetDataCancelledCallback();
    
    bool IsHitResultValid(const FHitResult& HitResult) const;

    void EndActivationIfComplete();

    FPSCombatShotTracker ShotTracker;
    
    FDelegateHandle OnTargetDataReadyCallbackHandle;
    FDelegateHandle OnTargetDataCancelledCallbackHandle;
    
    bool bHasTargetDataSent = false;
};
