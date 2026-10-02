// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/FPSCombatRangedWeaponInstance.h"
#include "AbilitySystem/Abilities/FPSCombatBaseGameplayAbility.h"
#include "FPSCombatGameplayAbilityReload.generated.h"

/**
 * 
 */
UCLASS()
class FPSCOMBATSYSTEM_API UFPSCombatGameplayAbilityReload : public UFPSCombatBaseGameplayAbility
{
	GENERATED_BODY()

	UFPSCombatGameplayAbilityReload();
	
public:
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr,
		FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;


protected:
	UFUNCTION(BlueprintCallable, Category = "Ability")
	UFPSCombatRangedWeaponInstance* GetWeaponInstance() const;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UFUNCTION(BlueprintCallable, Category = "Ability")
	void GrantReloadAmmo() const;
};
