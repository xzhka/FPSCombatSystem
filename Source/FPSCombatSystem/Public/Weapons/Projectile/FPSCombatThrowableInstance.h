// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Equipment/FPSCombatEquipmentInstance.h"
#include "Weapons/FPSCombatThrowableDefinition.h"
#include "FPSCombatThrowableInstance.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class FPSCOMBATSYSTEM_API UFPSCombatThrowableInstance : public UFPSCombatEquipmentInstance
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	UFPSCombatThrowableDefinition* GetThrowableDefinition() const ;

	UFUNCTION(BlueprintCallable)
	bool HasChargesRemaining() const;

	UFUNCTION(BlueprintCallable)
	void ConsumeProjectile(FGameplayTag ProjectileTag) const;
};
