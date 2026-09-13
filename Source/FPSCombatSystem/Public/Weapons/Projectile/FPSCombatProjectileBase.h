// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FPSCombatProjectileDefinition.h"
#include "GameplayEffectTypes.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "FPSCombatProjectileBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnProjectileImpact, AActor*, ImpactActor, const FHitResult&, Hit);


UCLASS()
class FPSCOMBATSYSTEM_API AFPSCombatProjectileBase : public AActor
{
	GENERATED_BODY()
	
public:
	/* Functions */

	
	AFPSCombatProjectileBase();


	void ApplyImpactCue(const FHitResult& ImpactResult);
	virtual void Tick(float DeltaSeconds) override;
	
	virtual void PostInitializeComponents() override;

	void InitializeProjectile(const FGameplayEffectSpecHandle& SpecHandle, AActor* InstigatorActor, UAbilitySystemComponent* ASC);

	void InitializeVelocity(const FVector& ProjectileVelocity);
	
	
	/* Getters and Setters */
	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetDestroyOnImpact(const bool bOnImpact) { bDestroyOnImpact = bOnImpact; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetApplyDirectDamageOnImpact(bool bInApply) { bApplyDirectDamageOnImpact = bInApply; }
	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE UProjectileMovementComponent* GetProjectileMovementComponent() { return MovementComp; } 
	
	FORCEINLINE UAbilitySystemComponent* GetSourceASC() const ;
	
	FOnProjectileImpact OnImpact;

protected:

	/* Functions */
	
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OnComponentBeginOverlap, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnProjectileBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity);
	
	void OnProjectileImpact(AActor* ImpactActor, const FHitResult& ImpactResult);
	void ApplyProjectileDefinition();
	void ApplyDamageToTarget(AActor* OtherActor) const;
	virtual void BeginPlay() override;

	/* Variables */
	
	UPROPERTY(VisibleDefaultsOnly, Category = "Projectile")
	TObjectPtr<USphereComponent> CollisionComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Projectile")
	TObjectPtr<UProjectileMovementComponent> MovementComp;

	UPROPERTY(EditDefaultsOnly, Category = "Definition")
	TObjectPtr<UFPSCombatProjectileDefinition> ProjectileDefinition;

	UPROPERTY(EditDefaultsOnly, Category = "Mesh")
	TObjectPtr<UStaticMeshComponent> StaticMeshComp;

	UPROPERTY(EditDefaultsOnly, Category = "Tags")
	FGameplayTag GameplayCueExplosionTag;
	
	UPROPERTY()
	FGameplayEffectSpecHandle DamageSpecHandle;

	TWeakObjectPtr<UAbilitySystemComponent> SourceASC;

	FVector SpinAxis = FVector::ZeroVector;

	float SpinRateDegPerSec = 0.0f;
	
private:

	/* Variables */
	
	bool bDestroyOnImpact = true;

	bool bApplyDirectDamageOnImpact = true;
	
	bool bHasImpacted = false;
};
