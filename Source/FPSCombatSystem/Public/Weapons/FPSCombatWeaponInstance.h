// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FPSCombatWeaponDefinition.h"

#include "Equipment/FPSCombatEquipmentInstance.h"
#include "FPSCombatWeaponInstance.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class FPSCOMBATSYSTEM_API UFPSCombatWeaponInstance : public UFPSCombatEquipmentInstance
{
	GENERATED_BODY()

	
public:
	virtual void OnEquipped() override;
	virtual void OnUnequipped() override;


	UFUNCTION(BlueprintCallable)
	void UpdateLastFireTime();

	UFUNCTION(BlueprintPure)
	float GetTimeFromLastInteraction() const;

	FORCEINLINE float GetTimeSinceLastFire() const { return GetWorld()->GetTimeSeconds()-TimeFired; }
	
	
private:

	float TimeEquipped = 0.f;
	float TimeFired = 0.f;
	
};
