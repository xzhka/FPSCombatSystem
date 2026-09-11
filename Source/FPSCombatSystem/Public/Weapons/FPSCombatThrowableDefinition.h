// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Equipment/FPSCombatEquipmentDefinition.h"
#include "Projectile/FPSCombatProjectileBase.h"
#include "Projectile/FPSCombatProjectileDefinition.h"
#include "FPSCombatThrowableDefinition.generated.h"

UENUM(BlueprintType)
enum class ESpawnActorDetonationTrigger : uint8
{
	OnFirstImpact,
	OnBounceCount, 
	OnFuseTimer
};

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class FPSCOMBATSYSTEM_API UFPSCombatThrowableDefinition : public UFPSCombatEquipmentDefinition
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Parameters")
	float ExplosionRadius = 500.f;

	UPROPERTY(EditDefaultsOnly, Category = "Parameters")
	TObjectPtr<UCurveFloat> FalloffCurve;

	UPROPERTY(EditDefaultsOnly, Category = "Parameters")
	float FuseDuration = 3.f;

	UPROPERTY(EditDefaultsOnly, Category = "Explosion")
	int32 BouncesBeforeDetonation = 4;

	UPROPERTY(EditDefaultsOnly, Category = "Parameters")
	ESpawnActorDetonationTrigger DetonationTrigger = ESpawnActorDetonationTrigger::OnFirstImpact;
	
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float BaseDamage = 50.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditDefaultsOnly, Category = "Classes")
	TSoftClassPtr<AFPSCombatProjectileBase> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category = "Duration")
	float ThrowsCooldown = 0.5f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Throw")
	float ThrowImpulse = 1500.f;

	UPROPERTY(EditDefaultsOnly, Category = "Throw")
	float LobImpulse = 500.f;

	UPROPERTY(EditDefaultsOnly, Category = "Throw")
	float LobArcAngle = 35.f;

	UPROPERTY(EditDefaultsOnly, Category = "Throw")
	float SpawnOffset = 80.f;

	UPROPERTY(EditDefaultsOnly, Category = "Throw")
	float SpawnHeightOffset = 10.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TSoftObjectPtr<UAnimMontage> ThrowMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TSoftObjectPtr<UAnimMontage> HoldMontage;
};
