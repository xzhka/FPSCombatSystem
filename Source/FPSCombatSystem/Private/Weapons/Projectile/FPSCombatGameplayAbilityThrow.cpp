// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Projectile/FPSCombatGameplayAbilityThrow.h"
#include "FPSCombatSystem/FPSCombatGameplayTags.h"
#include "Equipment/FPSCombatEquipmentManager.h"
#include "Equipment/FPSCombatQuickBarComponent.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Weapons/Projectile/Components/ProjectileComponent_Explosive.h"

UFPSCombatGameplayAbilityThrow::UFPSCombatGameplayAbilityThrow()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

bool UFPSCombatGameplayAbilityThrow::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}
	
	const UAbilitySystemComponent* ASC = ActorInfo ? ActorInfo->AbilitySystemComponent.Get() : nullptr;
	const FGameplayAbilitySpec* Spec = ASC ? ASC->FindAbilitySpecFromHandle(Handle) : nullptr;
	const UFPSCombatThrowableInstance* Instance = Spec ? Cast<UFPSCombatThrowableInstance>(Spec->SourceObject.Get()) : nullptr;

	return Instance && Instance->HasChargesRemaining();
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
	K2_OnThrowSetupComplete();
	
	WaitInputRelease  = UAbilityTask_WaitInputRelease::WaitInputRelease(this, false);
	WaitInputRelease->OnRelease.AddDynamic(this, &ThisClass::HandleInputReleased);
	WaitInputRelease->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* WaitGameplayEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, FPSCombatGameplayTags::Ability_ExternalResolveRequested, nullptr, false, true);
	WaitGameplayEvent->EventReceived.AddDynamic(this, &ThisClass::HandleExternalResolveEvent);
	WaitGameplayEvent->ReadyForActivation();
	
	GetWorld()->GetTimerManager().SetTimer(MaxHoldHandle, this, &UFPSCombatGameplayAbilityThrow::OnReleaseNotifyTimeout, MaxHoldTime, false);	

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

	ThrowInstance->ConsumeProjectile(FPSCombatGameplayTags::Data_Projectile_Quantity);
}

void UFPSCombatGameplayAbilityThrow::OnReleaseNotifyTimeout()
{
	FinalizeThrowReleased(MaxHoldTime);
}

void UFPSCombatGameplayAbilityThrow::HandleInputReleased(float TimeHeld)
{
	FinalizeThrowReleased(TimeHeld);
}

void UFPSCombatGameplayAbilityThrow::HandleExternalResolveEvent(FGameplayEventData Payload)
{
	if (!CurrentActorInfo && !CurrentActorInfo->IsNetAuthority())
	{
		return;
	}
	
	GetWorld()->GetTimerManager().ClearTimer(MaxHoldHandle);
	if (WaitInputRelease)
	{
		WaitInputRelease->EndTask();
		WaitInputRelease = nullptr;
	}

	SpawnAndLaunchProjectile();

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UFPSCombatGameplayAbilityThrow::SetThrowableVisualsActive(bool bActive)
{
	if (AController* Controller = GetController())
	{
		if (UFPSCombatQuickBarComponent* QuickBar = Controller->FindComponentByClass<UFPSCombatQuickBarComponent>())
		{
			if (UFPSCombatEquipmentInstance* ActiveItem = QuickBar->GetActiveItemInstance())
			{
				ActiveItem->SetEquipmentActorsHidden(bActive);
			}
		}
	}

	if (!IsValid(ThrowInstance))
	{
		return;
	}

	if (bActive)
	{
		ThrowInstance->SpawnEquipmentActorsFromInstance();
		ThrowInstance->SetEquipmentActorsHidden(false);
	}
	else
	{
		ThrowInstance->SetEquipmentActorsHidden(true);
	}
}

UAnimMontage* UFPSCombatGameplayAbilityThrow::BP_GetHoldMontage() const
{
	return GetThrowableDefinition() ? GetThrowableDefinition()->HoldMontage.LoadSynchronous() : nullptr;
}

UAnimMontage* UFPSCombatGameplayAbilityThrow::BP_GetThrowMontage() const
{
	return GetThrowableDefinition() ? GetThrowableDefinition()->ThrowMontage.LoadSynchronous() : nullptr;
}

void UFPSCombatGameplayAbilityThrow::FinalizeThrowReleased(float TimeHeld)
{
	if (CurrentActorInfo && CurrentActorInfo->IsNetAuthority())
	{
		GetWorld()->GetTimerManager().ClearTimer(MaxHoldHandle);
	}
	if (WaitInputRelease)
	{
		WaitInputRelease->EndTask();
		WaitInputRelease = nullptr;
	}
	
	K2_OnThrowReleased(TimeHeld);
}

void UFPSCombatGameplayAbilityThrow::EndAbility(const FGameplayAbilitySpecHandle Handle,
                                                const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                bool bReplicateEndAbility, bool bWasCancelled)
{
	GetWorld()->GetTimerManager().ClearTimer(MaxHoldHandle);
	SetThrowableVisualsActive(false);
	
	if (ThrowInstance)
	{
		ThrowInstance->ClearEquipmentActors();
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UFPSCombatGameplayAbilityThrow::CommitExecute(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	ApplyCost(Handle, ActorInfo, ActivationInfo);
}
