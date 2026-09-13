// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Weapons/FPSCombatThrowableDefinition.h"
#include "Weapons/Projectile/FPSCombatProjectileBase.h"
#include "ProjectileComponent_Explosive.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FPSCOMBATSYSTEM_API UProjectileComponent_Explosive : public UActorComponent
{
	GENERATED_BODY()

public:	

	/* Functions */
	
	void Detonate();

	void DetonateInternal(const FHitResult& Hit);

	void InitializeExplosion(const UFPSCombatThrowableDefinition* InThrowableDef);

	UFUNCTION()
	void OnProjectileImpact(AActor* ImpactActor, const FHitResult& Hit);
	
	virtual void BeginPlay() override;

protected:
	UPROPERTY()
	TObjectPtr<const UFPSCombatThrowableDefinition> ThrowableDef;
	
	UPROPERTY()
	TObjectPtr<AFPSCombatProjectileBase> OwnerProjectile;
	
	int32 CurrentBounces = 0;

	FTimerHandle FuseTimer;
	
	bool bAlreadyDetonated = false;

	UFUNCTION()
	void OnProjectileBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity);

	void SetupOwnerImpactBehaviour();
	void InitializeOwnerDetonationTrigger();
};
