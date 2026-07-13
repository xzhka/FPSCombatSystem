// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "FPSCombatBaseGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class FPSCOMBATSYSTEM_API UFPSCombatBaseGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:

	UFPSCombatBaseGameplayAbility();
	
	virtual const FGameplayTagContainer* GetCooldownTags() const override;
	virtual void ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo) const override;
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr,
		FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UPROPERTY(EditDefaultsOnly, Category = "Tags")
	FGameplayTagContainer AddedOnTags;

	UPROPERTY(EditDefaultsOnly, Category = "Tags")
	FGameplayTagContainer RemovedOnTags;
	
	UFUNCTION()
	void HandleCancelTagTriggered();
	

	UPROPERTY(EditDefaultsOnly, Category = "Activation")
	FGameplayTagQuery ActivationTagQuery;

	UFUNCTION(BlueprintCallable)
	void EffectSpecApply(TArray<TSubclassOf<UGameplayEffect>> GrantedEffectsClass, TArray<FActiveGameplayEffectHandle>& GrantedEffectHandle, FGameplayTag DurationSetByCallerTag = FGameplayTag(), float DurationValue = 0.f);

	UFUNCTION(BlueprintCallable)
	void EffectSpecRemove(TArray<FActiveGameplayEffectHandle> GrantedEffectHandle);
	
	UPROPERTY(Transient)
	FGameplayTagContainer TempCooldownTags;

	
	UPROPERTY(EditDefaultsOnly, Category = "Cooldown")
	FScalableFloat CooldownDuration;
	
	UPROPERTY(EditDefaultsOnly, Category = "Cooldown")
	FGameplayTagContainer CooldownTags;
	
	UPROPERTY(EditDefaultsOnly, Category = "Cooldown")
	FGameplayTag SetByCallerTag;
};
