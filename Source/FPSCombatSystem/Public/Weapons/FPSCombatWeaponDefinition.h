// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Equipment/FPSCombatEquipmentDefinition.h"
#include "FPSCombatWeaponDefinition.generated.h"

class UFPSCombatFireMode;

UCLASS(Blueprintable, BlueprintType)
class FPSCOMBATSYSTEM_API UFPSCombatWeaponDefinition : public UFPSCombatEquipmentDefinition
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float BaseDamage = 20.f;
	
	/* Ammo global parameters */
	UPROPERTY(EditDefaultsOnly, Category = "Ammo")
	int32 ReserveAmmo = 120;

	UPROPERTY(EditDefaultsOnly, Category = "Ammo")
	int32 ClipSize = 20;

	UPROPERTY(EditDefaultsOnly, Category = "Ammo")
	float ReloadDuration = 2.f;

	/* Fire characteristics */

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TSubclassOf<UGameplayEffect> DamageEffectClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Fire")
	TSubclassOf<UFPSCombatFireMode> FireModeClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Fire")
	float TraceRange = 10000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Fire")
	float HipFireSpread = 12.f;

	UPROPERTY(EditDefaultsOnly, Category = "Fire")
	float AimedMovingSpread = 8.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Fire")
	float DurationBetweenShoot = 0.2f;

	UPROPERTY(EditDefaultsOnly, Category = "Fire")
	float SpreadBiasExponent = 1.5f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Fire")
	TArray<FVector2D> RecoilPattern;
	
	UPROPERTY(EditDefaultsOnly, Category = "Fire")
	float RecoilResetDelay = 0.3f;

	UPROPERTY(EditDefaultsOnly, Category = "Fire")
	float MovementHipFireMultiplier = 1.5f;

	UPROPERTY(EditDefaultsOnly, Category = "Fire")
	float MaxSpreadDegrees = 40.f;

	/* Animation */

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	FName LeftHandGripSocket;
	
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TSoftObjectPtr<UAnimMontage> FireMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TSoftObjectPtr<UAnimMontage> PickUpMontage;
	
};