// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/FPSCombatAbilityTypes.h"
#include "FPSCombatFireMode.generated.h"

class UFPSCombatRangedWeaponInstance;
/**
 * 
 */
UCLASS(Abstract, Blueprintable, DefaultToInstanced)
class FPSCOMBATSYSTEM_API UFPSCombatFireMode : public UObject
{
	GENERATED_BODY()

public:
	virtual void OnInputPressed(UFPSCombatRangedWeaponInstance* Instance) {} 

	virtual void OnInputReleased(UFPSCombatRangedWeaponInstance* Instance) {}

	virtual void NotifyFireShot(UFPSCombatRangedWeaponInstance* Instance) {}

	virtual bool WantAttackNextShot(UFPSCombatRangedWeaponInstance* Instance) const { return false; }
	
	virtual EFPSCombatAbilityActivationPolicy GetActivationPolicy() const
	{
		return EFPSCombatAbilityActivationPolicy::OnInputTriggered;
	}
};

UCLASS()
class UFPSCombatFireMode_Burst : public UFPSCombatFireMode
{
	GENERATED_BODY()

public:
	virtual void OnInputPressed(UFPSCombatRangedWeaponInstance* Instance) override { CurrentBurst=0; }
	virtual void NotifyFireShot(UFPSCombatRangedWeaponInstance* Instance) override { ++CurrentBurst; }
	virtual bool WantAttackNextShot(UFPSCombatRangedWeaponInstance* Instance) const override { return CurrentBurst < BurstSize; }
private:
	UPROPERTY(EditDefaultsOnly, Category = "Fire")
	int32 BurstSize = 3;
	
	int32 CurrentBurst = 0;
};

UCLASS()
class UFPSCombatFireMode_Semi : public UFPSCombatFireMode
{
	GENERATED_BODY()
};

UCLASS()
class UFPSCombatFireMode_FullAuto : public UFPSCombatFireMode
{
	GENERATED_BODY()

public:
	virtual EFPSCombatAbilityActivationPolicy GetActivationPolicy() const override { return EFPSCombatAbilityActivationPolicy::WhileInputActive; }
};
