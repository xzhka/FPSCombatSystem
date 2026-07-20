// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Equipment/FPSCombatEquipmentDefinition.h"
#include "FPSCombatWeaponDefinition.generated.h"

UENUM(BlueprintType)
enum class EWeaponFireType : uint8
{
	Projectile,
	Hitscan
};

UENUM(BlueprintType)
enum class EWeaponShotType : uint8
{
	Semi,
	FullAuto,
	Burst
};


UCLASS()
class FPSCOMBATSYSTEM_API UFPSCombatWeaponDefinition : public UFPSCombatEquipmentDefinition
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float BaseDamage = 0.f;
	
	/* Ammo global parameters */
	UPROPERTY(EditDefaultsOnly, Category = "Ammo")
	int32 ReserveAmmo = 120;

	UPROPERTY(EditDefaultsOnly, Category = "Ammo")
	int32 ClipSize = 20;

	UPROPERTY(EditDefaultsOnly, Category = "Ammo")
	float ReloadDuration = 4.f;

	/* Fire characteristics */
	UPROPERTY(EditDefaultsOnly, Category = "Fire")
	EWeaponFireType WeaponFireType = EWeaponFireType::Hitscan;

	UPROPERTY(EditDefaultsOnly, Category = "Fire")
	EWeaponShotType ShotFireType = EWeaponShotType::FullAuto;
	
	UPROPERTY(EditDefaultsOnly, Category = "Fire")
	float TraceRange = 10000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Fire")
	float HipFireSpread = 2.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Fire")
	float DurationBetweenShoot = 0.2f;

	UPROPERTY(EditDefaultsOnly, Category = "Fire")
	TArray<FVector2D> RecoilPattern;
	
	UPROPERTY(EditDefaultsOnly, Category = "Fire")
	float RecoilResetDelay = 0.2f;

	UPROPERTY(EditDefaultsOnly, Category = "Fire")
	float MovementHipFireMultiplier = 1.1f;

	UPROPERTY(EditDefaultsOnly, Category = "Fire")
	float AimedMovingSpread = 0.3f;
	
	// TODO: Create a Projectile Base class and add it in here
	// UPROPERTY(EditDefaultsOnly, Category = "Fire", meta = (EditCondition="WeaponFireType==EWeaponFireType::Projectile"))
	// TSoftClassPtr<AFPSCombatProjectileBase> ProjectileClass;


	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSoftObjectPtr<UTexture2D> WeaponIcon;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	FText WeaponName;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TSoftObjectPtr<UAnimMontage> FireMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TSoftObjectPtr<UAnimMontage> PickUpMontage;
	
};
