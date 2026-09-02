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

void UFPSCombatGameplayAbilityThrow::InputReleased(const FGameplayAbilitySpecHandle Handle, 
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);
	UE_LOG(LogTemp, Warning, TEXT("InputReleased"));
}

void UFPSCombatGameplayAbilityThrow::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                     const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                     const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	if (FGameplayAbilitySpec* Spec = UGameplayAbility::GetCurrentAbilitySpec())
	{
		ThrowInstance = Cast<UFPSCombatThrowableInstance>(Spec->SourceObject.Get());
	}
	UFPSCombatThrowableDefinition* ThrowableDef = ThrowInstance ? ThrowInstance->GetThrowableDefinition() : nullptr;
	if (!ThrowableDef)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	WaitInputRelease  = UAbilityTask_WaitInputRelease::WaitInputRelease(this, false);
	WaitInputRelease->OnRelease.AddDynamic(this, &ThisClass::OnReleaseNotify);
	WaitInputRelease->ReadyForActivation();

	if (ActorInfo->IsNetAuthority())
	{
		GetWorld()->GetTimerManager().SetTimer(MaxHoldHandle, this, &UFPSCombatGameplayAbilityThrow::OnReleaseNotifyTimeout, MaxHoldTime, false);	
	}
}

void UFPSCombatGameplayAbilityThrow::OnReleaseNotify(float TimeHandle)
{
	SpawnAndLaunchProjectile();
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UFPSCombatGameplayAbilityThrow::SpawnAndLaunchProjectile()
{
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

void UFPSCombatGameplayAbilityThrow::OnReleaseNotifyTimeout()
{
	OnReleaseNotify(MaxHoldTime);
}

void UFPSCombatGameplayAbilityThrow::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	if (ActorInfo->IsNetAuthority())
	{
		GetWorld()->GetTimerManager().ClearTimer(MaxHoldHandle);
	}
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
