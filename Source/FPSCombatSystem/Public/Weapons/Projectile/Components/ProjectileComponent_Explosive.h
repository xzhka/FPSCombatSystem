// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "Components/ActorComponent.h"
#include "Weapons/Projectile/FPSCombatProjectileBase.h"
#include "ProjectileComponent_Explosive.generated.h"

UENUM(BlueprintType)
enum class ESpawnActorDetonationTrigger : uint8
{
	OnFirstImpact,
	OnBounceCount, 
	OnFuseTimer
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FPSCOMBATSYSTEM_API UProjectileComponent_Explosive : public UActorComponent
{
	GENERATED_BODY()

public:	
	UProjectileComponent_Explosive();

	void Detonate();

	void InitializeExplosion(float InBaseDamage, TSubclassOf<UGameplayEffect> InDamageEffectClass);

	UFUNCTION()
	void OnProjectileImpact(AActor* ImpactActor, const FHitResult& Hit);
	
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Parameters")
	float ExplosionRadius = 500.f;

	UPROPERTY()
	TObjectPtr<AFPSCombatProjectileBase> OwnerProjectile;
	
	UPROPERTY(EditDefaultsOnly, Category = "Parameters")
	TObjectPtr<UCurveFloat> FalloffCurve;

	UPROPERTY(EditDefaultsOnly, Category = "Parameters")
	float FuseDuration = 3.f;
	
	float RuntimeBaseDamage = 0.f;
	
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	bool bAlreadyDetonated = false;

	UPROPERTY(EditDefaultsOnly, Category = "Parameters")
	ESpawnActorDetonationTrigger DetonationTrigger = ESpawnActorDetonationTrigger::OnFirstImpact;
};
