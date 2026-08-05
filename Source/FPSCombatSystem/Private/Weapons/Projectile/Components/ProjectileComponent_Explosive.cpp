// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Projectile/Components/ProjectileComponent_Explosive.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Engine/OverlapResult.h"
#include "FPSCombatSystem/FPSCombatGameplayTags.h"

UProjectileComponent_Explosive::UProjectileComponent_Explosive()
{
	
}

void UProjectileComponent_Explosive::OnProjectileImpact(AActor* ImpactActor, const FHitResult& Hit)
{
	UE_LOG(LogTemp, Warning, TEXT("OnProjectileImpact"));
	switch (DetonationTrigger)
	{
	case ESpawnActorDetonationTrigger::OnFirstImpact:
		Detonate();
		break;
	case ESpawnActorDetonationTrigger::OnBounceCount:
		break;
	case ESpawnActorDetonationTrigger::OnFuseTimer:
		break;
	}
}

void UProjectileComponent_Explosive::Detonate()
{
	if (!GetOwner()->HasAuthority() || !OwnerProjectile) return;
	
	if (bAlreadyDetonated) return;
	bAlreadyDetonated = true;
	UE_LOG(LogTemp, Warning, TEXT("Detonate"));
	UAbilitySystemComponent* SourceASC = OwnerProjectile->GetSourceASC();
	if (!SourceASC) return;
	
	const FVector PosLocation = OwnerProjectile->GetActorLocation();

	FCollisionQueryParams Params(SCENE_QUERY_STAT(ExplosionOverlap), false);
	Params.AddIgnoredActor(OwnerProjectile);
	
	TArray<FOverlapResult> Overlaps;
	GetWorld()->OverlapMultiByChannel(Overlaps, PosLocation, FQuat::Identity, ECC_Pawn, FCollisionShape::MakeSphere(ExplosionRadius), Params);

	TSet<AActor*> ProcessedTargets;
	
	for (FOverlapResult& OverlapResult : Overlaps)
	{
		AActor* Target = OverlapResult.GetActor();
		if (!Target || ProcessedTargets.Contains(Target)) continue;

		ProcessedTargets.Add(Target);
		UE_LOG(LogTemp, Warning, TEXT("Detonate: Before TargetASC"));
		UAbilitySystemComponent* TargetASC = Target ? UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target) : nullptr;
		if (!TargetASC) continue;
		
		const float Distance = FVector::Dist(PosLocation, Target->GetActorLocation());
		const float Alpha = FMath::Clamp(Distance/ExplosionRadius, 0.0f, 1.0f);
		const float FalloffMul = FalloffCurve ? FalloffCurve->GetFloatValue(Alpha) : 1.f;
		
		FGameplayEffectContextHandle ContextHandle = SourceASC->MakeEffectContext();
		ContextHandle.AddSourceObject(OwnerProjectile);
		ContextHandle.AddInstigator(OwnerProjectile->GetInstigator(), OwnerProjectile);

		UE_LOG(LogTemp, Warning, TEXT("Detonate: Before SpecHandle. DamageEffectClass: %d, ContextHandle: %d"), DamageEffectClass != nullptr, ContextHandle.IsValid());
		FGameplayEffectSpecHandle SpecHandleData = SourceASC->MakeOutgoingSpec(DamageEffectClass, 1.f, ContextHandle);
		if (!SpecHandleData.IsValid()) continue;
		const float FinalMag = RuntimeBaseDamage * FalloffMul;
		UE_LOG(LogTemp, Warning, TEXT("UProjectileComponent_Explosive::Detonate - FinalMag: %f"), FinalMag);
		SpecHandleData.Data->SetSetByCallerMagnitude(FPSCombatGameplayTags::SetByCaller_Data_Damage, FinalMag);
		SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandleData.Data.Get(), TargetASC);
	}
}

void UProjectileComponent_Explosive::InitializeExplosion(float InBaseDamage,
	TSubclassOf<UGameplayEffect> InDamageEffectClass)
{
	UE_LOG(LogTemp, Warning, TEXT("UProjectileComponent_Explosive::InitializeExplosion"));
	RuntimeBaseDamage = InBaseDamage;
	DamageEffectClass = InDamageEffectClass;
}

void UProjectileComponent_Explosive::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("UProjectileComponent_Explosive::BeginPlay"));
	OwnerProjectile = Cast<AFPSCombatProjectileBase>(GetOwner());
	if (!OwnerProjectile) return;

	OwnerProjectile->OnImpact.AddDynamic(this, &ThisClass::OnProjectileImpact);

	const bool bDetonatesInstantly = (DetonationTrigger == ESpawnActorDetonationTrigger::OnFirstImpact);

	OwnerProjectile->SetApplyDirectDamageOnImpact(!bDetonatesInstantly);
	
	if (!bDetonatesInstantly)
	{
		OwnerProjectile->SetDestroyOnImpact(false);
	}
}

