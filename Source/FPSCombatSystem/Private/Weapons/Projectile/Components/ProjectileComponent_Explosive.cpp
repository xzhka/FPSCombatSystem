// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Projectile/Components/ProjectileComponent_Explosive.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Engine/OverlapResult.h"
#include "FPSCombatSystem/FPSCombatGameplayTags.h"



void UProjectileComponent_Explosive::OnProjectileImpact(AActor* ImpactActor, const FHitResult& Hit)
{
	if (ThrowableDef)
	{
		if (ThrowableDef->DetonationTrigger == ESpawnActorDetonationTrigger::OnFirstImpact)
		{
			DetonateInternal(Hit);
		}
	}
}

void UProjectileComponent_Explosive::Detonate()
{
	if (!OwnerProjectile) return;

	const FVector Loc = OwnerProjectile->GetActorLocation();

	FHitResult ImpactHit;
	ImpactHit.Location = Loc;
	ImpactHit.ImpactPoint = Loc;
	ImpactHit.ImpactNormal = FVector::UpVector;
	ImpactHit.Normal = FVector::UpVector;

	DetonateInternal(ImpactHit);
}

void UProjectileComponent_Explosive::DetonateInternal(const FHitResult& Hit)
{
	if (bAlreadyDetonated || !GetOwner()->HasAuthority() || !OwnerProjectile) return;
	bAlreadyDetonated = true;

	OwnerProjectile->ApplyImpactCue(Hit);
	
	GetWorld()->GetTimerManager().ClearTimer(FuseTimer);
	if (UProjectileMovementComponent* ProjMoveComp = OwnerProjectile->GetProjectileMovementComponent())
	{
		ProjMoveComp->OnProjectileBounce.RemoveDynamic(this, &ThisClass::OnProjectileBounce);
	}
	
	UAbilitySystemComponent* SourceASC = OwnerProjectile->GetSourceASC();
	if (SourceASC)
	{
		const FVector PosLocation = OwnerProjectile->GetActorLocation();

		FCollisionQueryParams Params(SCENE_QUERY_STAT(ExplosionOverlap), false);
		Params.AddIgnoredActor(OwnerProjectile);
	
		TArray<FOverlapResult> Overlaps;
		GetWorld()->OverlapMultiByChannel(Overlaps, PosLocation, FQuat::Identity, ECC_Pawn, FCollisionShape::MakeSphere(ThrowableDef->ExplosionRadius), Params);
		
		TSet<AActor*> ProcessedTargets;
	
		for (FOverlapResult& OverlapResult : Overlaps)
		{
			AActor* Target = OverlapResult.GetActor();
			if (!Target || ProcessedTargets.Contains(Target)) continue;

			ProcessedTargets.Add(Target);
			UAbilitySystemComponent* TargetASC = Target ? UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target) : nullptr;
			if (!TargetASC) continue;
		
			const float Distance = FVector::Dist(PosLocation, Target->GetActorLocation());
			const float Alpha = FMath::Clamp(Distance/ThrowableDef->ExplosionRadius, 0.0f, 1.0f);
			const float FalloffMul = ThrowableDef->FalloffCurve ? ThrowableDef->FalloffCurve->GetFloatValue(Alpha) : 1.f;
		
			FGameplayEffectContextHandle ContextHandle = SourceASC->MakeEffectContext();
			ContextHandle.AddSourceObject(OwnerProjectile);
			ContextHandle.AddInstigator(OwnerProjectile->GetInstigator(), OwnerProjectile);
		
			FGameplayEffectSpecHandle SpecHandleData = SourceASC->MakeOutgoingSpec(ThrowableDef->DamageEffectClass, 1.f, ContextHandle);
			if (!SpecHandleData.IsValid()) continue;
			const float FinalMag = ThrowableDef->BaseDamage * FalloffMul;
			SpecHandleData.Data->SetSetByCallerMagnitude(FPSCombatGameplayTags::SetByCaller_Data_Damage, FinalMag);
			SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandleData.Data.Get(), TargetASC);
		}
	}
	
	OwnerProjectile->Destroy();
}

void UProjectileComponent_Explosive::InitializeExplosion(const UFPSCombatThrowableDefinition* InThrowableDef)
{
	if (InThrowableDef)
	{
		ThrowableDef = InThrowableDef;
	}

	if (!ThrowableDef || !OwnerProjectile) return;
	
	SetupOwnerImpactBehaviour();
	InitializeOwnerDetonationTrigger();
}

void UProjectileComponent_Explosive::BeginPlay()
{
	Super::BeginPlay();
	OwnerProjectile = Cast<AFPSCombatProjectileBase>(GetOwner());
	if (!OwnerProjectile) return;

	OwnerProjectile->OnImpact.AddDynamic(this, &ThisClass::OnProjectileImpact);
}

void UProjectileComponent_Explosive::OnProjectileBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity)
{
	if (ThrowableDef)
	{
		if (++CurrentBounces >= ThrowableDef->BouncesBeforeDetonation)
		{
			DetonateInternal(ImpactResult);
		}
	}
}

void UProjectileComponent_Explosive::SetupOwnerImpactBehaviour()
{
	const bool bDetonatesInstantly = (ThrowableDef->DetonationTrigger == ESpawnActorDetonationTrigger::OnFirstImpact);

	OwnerProjectile->SetApplyDirectDamageOnImpact(!bDetonatesInstantly);
	OwnerProjectile->SetDestroyOnImpact(false);
}

void UProjectileComponent_Explosive::InitializeOwnerDetonationTrigger()
{
	if (!OwnerProjectile || !ThrowableDef) return;
	
	switch (ThrowableDef->DetonationTrigger)
	{
	case ESpawnActorDetonationTrigger::OnBounceCount:
		if (UProjectileMovementComponent* ProjMoveComp = OwnerProjectile->GetProjectileMovementComponent())
		{
			ProjMoveComp->OnProjectileBounce.AddDynamic(this, &ThisClass::OnProjectileBounce);
		}
		break;
	case ESpawnActorDetonationTrigger::OnFuseTimer:
		GetWorld()->GetTimerManager().SetTimer(FuseTimer, this, &UProjectileComponent_Explosive::Detonate, ThrowableDef->FuseDuration, false);
		break;
	default:
		break;
	}
}
