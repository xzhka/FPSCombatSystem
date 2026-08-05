// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Projectile//FPSCombatGameplayAbilityThrow.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "FPSCombatSystem/FPSCombatGameplayTags.h"
#include "Weapons/Projectile/Components/ProjectileComponent_Explosive.h"

UFPSCombatGameplayAbilityThrow::UFPSCombatGameplayAbilityThrow()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	ReleaseEventTag = FPSCombatGameplayTags::Ability_Throwable_Release;
	
}

void UFPSCombatGameplayAbilityThrow::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                     const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                     const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UE_LOG(LogTemp, Warning, TEXT("UFPSCombatGameplayAbilityThrow::ActivateAbility"));
	
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		UE_LOG(LogTemp, Warning, TEXT("UFPSCombatGameplayAbilityThrow::ActivateAbility - CommitAbility"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	if (FGameplayAbilitySpec* Spec = UGameplayAbility::GetCurrentAbilitySpec())
	{
		UE_LOG(LogTemp, Warning, TEXT("UFPSCombatGameplayAbilityThrow::ActivateAbility - ThrowInstance"));
		ThrowInstance = Cast<UFPSCombatThrowableInstance>(Spec->SourceObject.Get());
	}
	UFPSCombatThrowableDefinition* ThrowableDef = ThrowInstance ? ThrowInstance->GetThrowableDefinition() : nullptr;
	if (!ThrowableDef)
	{
		UE_LOG(LogTemp, Warning, TEXT("UFPSCombatGameplayAbilityThrow::ActivateAbility - !ThrowableDef"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}


	SpawnAndLaunchProjectile();
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	// UE_LOG(LogTemp, Warning, TEXT("UFPSCombatGameplayAbilityThrow::ActivateAbility- UAbilityTask_WaitGameplayEvent"));
	// UAbilityTask_WaitGameplayEvent* WaitReleaseEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, ReleaseEventTag, nullptr, false, false);
	// WaitReleaseEvent->EventReceived.AddDynamic(this, &UFPSCombatGameplayAbilityThrow::OnReleaseNotify);
	// WaitReleaseEvent->ReadyForActivation();
}

void UFPSCombatGameplayAbilityThrow::OnReleaseNotify(FGameplayEventData Payload)
{
	SpawnAndLaunchProjectile();
}

void UFPSCombatGameplayAbilityThrow::SpawnAndLaunchProjectile()
{
	UE_LOG(LogTemp, Warning, TEXT("SpawnAndLaunchProjectile"));
	if (!HasAuthority(&CurrentActivationInfo)) return;

	APawn* Pawn = Cast<APawn>(GetAvatarActorFromActorInfo());

	if (!Pawn || !ThrowInstance) return;

	AController* Controller = Pawn->GetController();
	if (!Controller) return;

	FVector LocDir;
	FRotator LocRot;

	Controller->GetPlayerViewPoint(LocDir, LocRot);

	FVector AimDir = LocRot.Vector();
	
	UFPSCombatThrowableDefinition* ThrowableDefinition = ThrowInstance ? ThrowInstance->GetThrowableDefinition() : nullptr;
	TSubclassOf<AFPSCombatProjectileBase> ProjectileClass = ThrowableDefinition->ProjectileClass.LoadSynchronous();
	if (!ProjectileClass) return;


	FVector LaunchDir = AimDir;
	float LaunchSpeed = ThrowableDefinition->ThrowImpulse;

	FVector SpawnLoc = LocDir + AimDir * SpawnOffset + FVector(0.f, 0.f, 10.f);
	FTransform ShotTransform(LaunchDir.Rotation(), SpawnLoc);


	FActorSpawnParameters SpawnParams;
	SpawnParams.Instigator = Pawn;
	SpawnParams.Owner = Pawn;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AFPSCombatProjectileBase* Projectile = GetWorld()->SpawnActorDeferred<AFPSCombatProjectileBase>(ProjectileClass, ShotTransform, Pawn, Pawn, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (!Projectile) return;

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	
	FGameplayEffectSpecHandle DamageSpec = MakeOutgoingGameplayEffectSpec(ThrowableDefinition->DamageEffectClass, GetAbilityLevel());
	if (DamageSpec.IsValid())
	{
		DamageSpec.Data->SetSetByCallerMagnitude(FPSCombatGameplayTags::SetByCaller_Data_Damage, ThrowableDefinition->BaseDamage);
		Projectile->InitializeProjectile(DamageSpec, Pawn, ASC);
	}
	
	Projectile->InitializeVelocity(LaunchDir*LaunchSpeed);
	
	Projectile->FinishSpawning(ShotTransform);

	if (UProjectileComponent_Explosive* ExplosiveComp = Projectile->FindComponentByClass<UProjectileComponent_Explosive>())
	{
		ExplosiveComp->InitializeExplosion(ThrowableDefinition->BaseDamage, ThrowableDefinition->DamageEffectClass);
	}
}