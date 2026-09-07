// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Projectile/FPSCombatGameplayAbilityThrow.h"
#include "FPSCombatSystem/FPSCombatGameplayTags.h"
#include "Equipment/FPSCombatEquipmentManager.h"
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

	if (APawn* Pawn = Cast<APawn>(GetAvatarActorFromActorInfo()))
	{
		if (UFPSCombatEquipmentManager* EquipManager = Pawn->FindComponentByClass<UFPSCombatEquipmentManager>())
		{
			if (UFPSCombatEquipmentInstance* EquipInstance = EquipManager->GetFirstInstanceOfType<UFPSCombatEquipmentInstance>())
			{
				CachedItemInstance = EquipInstance;
				EquipInstance->SetEquipmentActorsHidden(true);
			}
		}
	}
	K2_OnThrowSetupComplete();
	
	WaitInputRelease  = UAbilityTask_WaitInputRelease::WaitInputRelease(this, false);
	WaitInputRelease->OnRelease.AddDynamic(this, &ThisClass::HandleInputReleased);
	WaitInputRelease->ReadyForActivation();

	if (ActorInfo->IsNetAuthority())
	{
		GetWorld()->GetTimerManager().SetTimer(MaxHoldHandle, this, &UFPSCombatGameplayAbilityThrow::OnReleaseNotifyTimeout, MaxHoldTime, false);	
	}
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
	
	
	UFPSCombatThrowableDefinition* ThrowableDefinition = ThrowInstance ? ThrowInstance->GetThrowableDefinition() : nullptr;
	TSubclassOf<AFPSCombatProjectileBase> ProjectileClass = ThrowableDefinition ? ThrowableDefinition->ProjectileClass.LoadSynchronous() : nullptr;
	if (!ProjectileClass) return;

	FVector AimDir = LocRot.Vector();
	FVector LaunchDir = AimDir;
	float LaunchSpeed = ThrowableDefinition->ThrowImpulse;

	FVector SpawnLoc = LocDir + AimDir * ThrowableDefinition->SpawnOffset + FVector(0.f, 0.f, ThrowableDefinition->SpawnHeightOffset);
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
		ExplosiveComp->InitializeExplosion(ThrowableDefinition);
	}
}

void UFPSCombatGameplayAbilityThrow::OnReleaseNotifyTimeout()
{
	K2_OnThrowReleased(MaxHoldTime);
}

void UFPSCombatGameplayAbilityThrow::HandleInputReleased(float TimeHeld)
{
	if (CurrentActorInfo && CurrentActorInfo->IsNetAuthority())
	{
		GetWorld()->GetTimerManager().ClearTimer(MaxHoldHandle);
	}
	K2_OnThrowReleased(TimeHeld);
}

UAnimMontage* UFPSCombatGameplayAbilityThrow::BP_GetHoldMontage() const
{
	return GetThrowableDefinition() ? GetThrowableDefinition()->HoldMontage.LoadSynchronous() : nullptr;
}

UAnimMontage* UFPSCombatGameplayAbilityThrow::BP_GetThrowMontage() const
{
	return GetThrowableDefinition() ? GetThrowableDefinition()->ThrowMontage.LoadSynchronous() : nullptr;
}

void UFPSCombatGameplayAbilityThrow::EndAbility(const FGameplayAbilitySpecHandle Handle,
                                                const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                bool bReplicateEndAbility, bool bWasCancelled)
{
	if (ActorInfo->IsNetAuthority())
	{
		GetWorld()->GetTimerManager().ClearTimer(MaxHoldHandle);
	}
	if (ThrowInstance)
	{
		ThrowInstance->ClearEquipmentActors();
	}
	if (CachedItemInstance)
	{
		CachedItemInstance->SetEquipmentActorsHidden(false);
		CachedItemInstance = nullptr;
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UFPSCombatGameplayAbilityThrow::CommitExecute(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	ApplyCost(Handle, ActorInfo, ActivationInfo);
}
