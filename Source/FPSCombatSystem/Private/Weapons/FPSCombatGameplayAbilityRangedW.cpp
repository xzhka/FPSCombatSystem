// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/FPSCombatGameplayAbilityRangedW.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "FPSCombatSystem/FPSCombatCollisionChannels.h"
#include "FPSCombatSystem/FPSCombatGameplayTags.h"

UFPSCombatGameplayAbilityRangedW::UFPSCombatGameplayAbilityRangedW()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

bool UFPSCombatGameplayAbilityRangedW::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                          const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
                                                          const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	if (UFPSCombatRangedWeaponInstance* RangedWeapon = GetWeaponInstance())
	{
		return RangedWeapon->CanFire();
	}
	return false;
}

void UFPSCombatGameplayAbilityRangedW::InputPressed(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);
	bWantsToFire = true;
}

void UFPSCombatGameplayAbilityRangedW::InputReleased(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);
	bWantsToFire = false;
}

UFPSCombatRangedWeaponInstance* UFPSCombatGameplayAbilityRangedW::GetWeaponInstance() const
{
	if (FGameplayAbilitySpec* Spec = UGameplayAbility::GetCurrentAbilitySpec())
	{
		return Cast<UFPSCombatRangedWeaponInstance>(Spec->SourceObject.Get());
	}
	return nullptr;
}

void UFPSCombatGameplayAbilityRangedW::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                       const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                       const FGameplayEventData* TriggerEventData)
{
	UFPSCombatRangedWeaponInstance* WeaponData = GetWeaponInstance();
	if (!WeaponData || !WeaponData->CanFire())
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}
	
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	bWantsToFire = true;

	UAbilitySystemComponent* ASC = CurrentActorInfo->AbilitySystemComponent.Get();
	check(ASC);

	ASC->AbilityTargetDataSetDelegate(CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey()).AddUObject(this, &ThisClass::OnTargetDataReadyCallback);
	
	WaitInputReleaseTask = UAbilityTask_WaitInputRelease::WaitInputRelease(this, false);
	WaitInputReleaseTask->OnRelease.AddDynamic(this, &ThisClass::OnInputReleased);
	WaitInputReleaseTask->ReadyForActivation();

	
	HandleFireInput();
}

void UFPSCombatGameplayAbilityRangedW::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	bWantsToFire = false;
	if (WaitDelayTask)
	{
		WaitDelayTask->EndTask();
		WaitDelayTask = nullptr;
	}
	if (WaitInputReleaseTask)
	{
		WaitInputReleaseTask->EndTask();
		WaitInputReleaseTask = nullptr;
	}
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UFPSCombatGameplayAbilityRangedW::StartRangedWeaponTargeting()
{
	UAbilitySystemComponent* ASC = CurrentActorInfo->AbilitySystemComponent.Get();
	check(ASC);
	
	const bool bIsControlled = CurrentActorInfo->IsLocallyControlled();
	const bool bIsAuthority = CurrentActorInfo->IsNetAuthority();

	if (bIsControlled)
	{
		TArray<FHitResult> Hits;

		PerformLocalTargeting(Hits);

		FGameplayAbilityTargetDataHandle TargetData;

		for (const FHitResult& FoundHitResult : Hits)
		{
			FGameplayAbilityTargetData_SingleTargetHit* TargetHit = new FGameplayAbilityTargetData_SingleTargetHit();
			TargetHit->HitResult = FoundHitResult;
			TargetData.Add(TargetHit);
		}
		if (bIsAuthority)
		{
			OnTargetDataReadyCallback(TargetData, FGameplayTag());
		}
		else
		{
			ASC->CallServerSetReplicatedTargetData(CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey(), TargetData, FGameplayTag(), ASC->ScopedPredictionKey);
		}
	}
	ASC->CallAllReplicatedDelegatesIfSet(CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey());
}

void UFPSCombatGameplayAbilityRangedW::PerformLocalTargeting(TArray<FHitResult>& OutHits)
{
	UFPSCombatRangedWeaponInstance* WeaponInstance = GetWeaponInstance();
	APawn* Pawn = GetAvatarActorFromActorInfo() ? Cast<APawn>(GetAvatarActorFromActorInfo()) : nullptr;

	if (!Pawn || !WeaponInstance) return;

	AController* Controller = Pawn->GetController();

	if (!Controller) return;

	FVector LocDir;
	FRotator LocRot;

	Controller->GetPlayerViewPoint(LocDir, LocRot);

	FVector AimDir = LocRot.Vector();
	FFPSCombatShotContext ShotContext = WeaponInstance->MakeShotContext();
	FVector ShotDir = WeaponInstance->CalculateFireDirection(ShotContext, AimDir);

	FVector EndDir = LocDir + ShotDir * WeaponInstance->GetWeaponDefinition()->TraceRange;

	FCollisionQueryParams Params(SCENE_QUERY_STAT(WeaponTrace), true, Pawn);
	Params.AddIgnoredActor(Pawn);

	ECollisionChannel TraceChannel = DetermineTraceChannel();
	
	
	FHitResult Hit;
	if (GetWorld()->LineTraceSingleByChannel(Hit, LocDir, EndDir, TraceChannel, Params))
	{
		DrawDebugLine(GetWorld(), LocDir, Hit.bBlockingHit ? Hit.ImpactPoint : EndDir, FColor::Red, false, 10.0f, 0, 1.f);
		OutHits.Add(Hit);
	}
	WeaponInstance->RegisterShotFired(ShotContext);
}

void UFPSCombatGameplayAbilityRangedW::OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& DataHandle,
	FGameplayTag ApplicationTag)
{
	UAbilitySystemComponent* ASC = CurrentActorInfo->AbilitySystemComponent.Get();

	UFPSCombatRangedWeaponInstance* WeaponInstance = GetWeaponInstance();

	if (ASC && CurrentActorInfo->IsNetAuthority() && WeaponInstance)
	{
		WeaponInstance->ConsumeRound();

		WeaponInstance->UpdateLastFireTime();
		
		if(DamageEffectClass)
		{
			FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass, GetAbilityLevel());
			SpecHandle.Data->SetSetByCallerMagnitude(FPSCombatGameplayTags::SetByCaller_Data_Damage,
				WeaponInstance->GetWeaponDefinition()->BaseDamage);
			for (auto It = DataHandle.Data.CreateConstIterator(); It; ++It)
			{
				if (const FHitResult* HitResult = DataHandle.Get(It.GetIndex())->GetHitResult())
				{
					if (!IsHitResultValid(*HitResult))
					{
						continue;
					}
					if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitResult->GetActor()))
					{
						ASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
					}
				}
			}
		}
	}
	ASC->ConsumeClientReplicatedTargetData(CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey());
}

ECollisionChannel UFPSCombatGameplayAbilityRangedW::DetermineTraceChannel() const
{
	return FPSCombat_TraceChannel_Weapon;
}

bool UFPSCombatGameplayAbilityRangedW::IsHitResultValid(const FHitResult& HitResult) const
{
	UFPSCombatRangedWeaponInstance* WeaponInstance = GetWeaponInstance();
	APawn* Pawn = GetAvatarActorFromActorInfo() ? Cast<APawn>(GetAvatarActorFromActorInfo()) : nullptr;

	if (!Pawn || !WeaponInstance) return false;

	AController* Controller = Pawn->GetController();

	if (!Controller) return false;
	
	FVector ViewLoc;
	FRotator LocRot;

	Controller->GetPlayerViewPoint(ViewLoc, LocRot);

	const float MaxRange = FMath::Square(WeaponInstance->GetWeaponDefinition()->TraceRange + HitValidationRangeSlack);
	if (FVector::DistSquared(ViewLoc, HitResult.Location) > MaxRange)
	{
		return false;
	}

	FCollisionQueryParams Params(SCENE_QUERY_STAT(WeaponValidation), true, Pawn);
	Params.AddIgnoredActor(Pawn);
	if (HitResult.GetActor())
	{
		Params.AddIgnoredActor(HitResult.GetActor());
	}

	ECollisionChannel TraceChannel = DetermineTraceChannel();
	
	FHitResult ServerHit;
	if (GetWorld()->LineTraceSingleByChannel(ServerHit, ViewLoc, HitResult.Location, TraceChannel, Params))
	{
		const float Tolerance = FMath::Square(HitValidationTolerance);
		if (FVector::DistSquared(ServerHit.Location, HitResult.Location) > Tolerance)
		{
			return false;
		}
	}
	return true;
}

void UFPSCombatGameplayAbilityRangedW::OnInputReleased(float TimeHeld)
{
	bWantsToFire = false;
}

void UFPSCombatGameplayAbilityRangedW::HandleFireInput()
{
	UFPSCombatRangedWeaponInstance* WeaponData = GetWeaponInstance();
	if (!WeaponData || !WeaponData->CanFire())
	{
		if (WeaponData && !WeaponData->HasAmmoInMag() && OnOutOfAmmo.IsValid())
		{
			FGameplayEventData Payload;
			Payload.EventTag = OnOutOfAmmo;

			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetAvatarActorFromActorInfo(), OnOutOfAmmo, Payload);
		}
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;
	}

	StartRangedWeaponTargeting();
	
	const EWeaponShotType ShotType = WeaponData->GetWeaponDefinition()->ShotFireType;

	if (ShotType != EWeaponShotType::FullAuto)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;
	}

	if (bWantsToFire)
	{
		HandleNextShot();
	}
	else
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}

void UFPSCombatGameplayAbilityRangedW::HandleNextShot()
{
	UFPSCombatRangedWeaponInstance* WeaponData = GetWeaponInstance();
	if (!WeaponData) return;
	WaitDelayTask = UAbilityTask_WaitDelay::WaitDelay(this, WeaponData->GetWeaponDefinition()->DurationBetweenShoot);
	WaitDelayTask->OnFinish.AddDynamic(this, &ThisClass::UFPSCombatGameplayAbilityRangedW::OnShotDelayFinished);
	WaitDelayTask->ReadyForActivation();
}

void UFPSCombatGameplayAbilityRangedW::OnShotDelayFinished()
{
	if (!bWantsToFire)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;
	}
	HandleFireInput();
}