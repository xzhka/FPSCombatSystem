// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FPSCombatProjectileDefinition.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class FPSCOMBATSYSTEM_API UFPSCombatProjectileDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float ProjectileSpeed = 3000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float ProjectileBounciness = 0.3f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float ProjectileGravityScale = 1.5f;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float ProjectileFriction = 0.7f;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	bool bProjectileBounceAffectFriction = true;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float ProjectileMinFrictionFraction = 0.3f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Lifetime")
	float ProjectileLifeSpan = 5.f;
};