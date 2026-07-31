// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/FPSCombatGameplayAbilityRangedW.h"

#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "FPSCombatSystem/FPSCombatCollisionChannels.h"
#include "FPSCombatSystem/FPSCombatGameplayTags.h"

UFPSCombatGameplayAbilityRangedW::UFPSCombatGameplayAbilityRangedW()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	
	ActivationPolicy = EFPSCombatAbilityActivationPolicy::OnInputTriggered;
}

bool UFPSCombatGameplayAbilityRangedW::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                          const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
                                                          const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	UAbilitySystemComponent* ASC = ActorInfo ? ActorInfo->AbilitySystemComponent.Get() : nullptr;
	const FGameplayAbilitySpec* Spec = ASC ? ASC->FindAbilitySpecFromHandle(Handle) : nullptr;
	UFPSCombatRangedWeaponInstance* WeaponData = Spec ? Cast<UFPSCombatRangedWeaponInstance>(Spec->SourceObject.Get()) : nullptr;

	if (!WeaponData || !WeaponData->CanFire())
	{
		return false;
	}

	return true;
}

void UFPSCombatGameplayAbilityRangedW::CancelAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateCancelAbility)
{
	if (ActorInfo->IsLocallyControlled() && !ActorInfo->IsNetAuthority() && !bHasTargetDataSent)
	{
		if (UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get())
		{
			ASC->ServerSetReplicatedTargetDataCancelled(Handle, ActivationInfo.GetActivationPredictionKey(), ASC->ScopedPredictionKey);
		}
	}
	
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

EFPSCombatAbilityActivationPolicy UFPSCombatGameplayAbilityRangedW::GetActivationPolicy(
	const FGameplayAbilitySpec& Spec) const
{
	if (UFPSCombatRangedWeaponInstance* WeaponInstance = Cast<UFPSCombatRangedWeaponInstance>(Spec.SourceObject.Get()))
	{
		if (UFPSCombatFireMode* FireMode = WeaponInstance->GetFireMode())
		{
			return FireMode->GetActivationPolicy();
		}
	}
	
	return Super::GetActivationPolicy(Spec);
}

UFPSCombatRangedWeaponInstance* UFPSCombatGameplayAbilityRangedW::GetWeaponInstance() const
{
	if (FGameplayAbilitySpec* Spec = GetCurrentAbilitySpec())
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

	WeaponData->HandleInputPressed();
	
	UAbilitySystemComponent* ASC = CurrentActorInfo->AbilitySystemComponent.Get();
	check(ASC);

	bHasTargetDataSent = false;
	OnTargetDataReadyCallbackHandle = ASC->AbilityTargetDataSetDelegate(CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey()).AddUObject(this, &ThisClass::OnTargetDataReadyCallback);
	OnTargetDataCancelledCallbackHandle = ASC->AbilityTargetDataCancelledDelegate(CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey()).AddUObject(this, &ThisClass::OnTargetDataCancelledCallback);
	
	HandleFireInput();
}

void UFPSCombatGameplayAbilityRangedW::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	UAbilitySystemComponent* ASC = CurrentActorInfo->AbilitySystemComponent.Get();
	ASC->AbilityTargetDataSetDelegate(CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey()).Remove(OnTargetDataReadyCallbackHandle);
	ASC->AbilityTargetDataCancelledDelegate(CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey()).Remove(OnTargetDataCancelledCallbackHandle);
	ASC->ConsumeClientReplicatedTargetData(CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey());
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UFPSCombatGameplayAbilityRangedW::NotifyInputReleased(const FGameplayAbilitySpec& Spec)
{
	if (UFPSCombatRangedWeaponInstance* WeaponInstance = Cast<UFPSCombatRangedWeaponInstance>(Spec.SourceObject.Get()))
	{
		WeaponInstance->HandleInputReleased();
	}
}

void UFPSCombatGameplayAbilityRangedW::StartRangedWeaponTargeting()
{
	check(CurrentActorInfo);

	UAbilitySystemComponent* ASC = CurrentActorInfo->AbilitySystemComponent.Get();
	check(ASC);

	APawn* Pawn = Cast<APawn>(GetAvatarActorFromActorInfo());
	const bool bIsLocallyControlled = Pawn && Pawn->IsLocallyControlled();

	if (CurrentActorInfo->IsNetAuthority() && !bIsLocallyControlled) { return; }
	
	TArray<FHitResult> Hits;
	PerformLocalTargeting(Hits);

	FGameplayAbilityTargetDataHandle TargetData;

	for (const FHitResult& FoundHitResult : Hits)
	{
		FGameplayAbilityTargetData_SingleTargetHit* TargetHit = new FGameplayAbilityTargetData_SingleTargetHit();
		TargetHit->HitResult = FoundHitResult;
		TargetData.Add(TargetHit);
	}

	OnTargetDataReadyCallback(TargetData, FGameplayTag());
}

void UFPSCombatGameplayAbilityRangedW::PerformLocalTargeting(TArray<FHitResult>& OutHits)
{
	UFPSCombatRangedWeaponInstance* WeaponInstance = GetWeaponInstance();
	APawn* Pawn = GetAvatarActorFromActorInfo() ? Cast<APawn>(GetAvatarActorFromActorInfo()) : nullptr;

	if (!Pawn || !WeaponInstance) return;


	if (Pawn && WeaponInstance && Pawn->IsLocallyControlled())
	{
		AController* Controller = Pawn->GetController();

		if (!Controller) return;

		FVector LocDir;
		FRotator LocRot;

		Controller->GetPlayerViewPoint(LocDir, LocRot);

		FVector AimDir = LocRot.Vector();
		const FFPSCombatShotContext ShotContext = WeaponInstance->MakeShotContext();
		const FVector ShotDir = WeaponInstance->CalculateFireDirection(ShotContext, AimDir);

		const FVector EndDir = LocDir + ShotDir * WeaponInstance->GetWeaponDefinition()->TraceRange;

		FCollisionQueryParams Params(SCENE_QUERY_STAT(WeaponTrace), true, Pawn);
		Params.AddIgnoredActor(Pawn);

		const ECollisionChannel TraceChannel = DetermineTraceChannel();
	
		FHitResult Hit;
		if (GetWorld()->LineTraceSingleByChannel(Hit, LocDir, EndDir, TraceChannel, Params))
		{
			DrawDebugLine(GetWorld(), LocDir, Hit.bBlockingHit ? Hit.ImpactPoint : EndDir, FColor::Red, false, 10.0f, 0, 1.f);
			OutHits.Add(Hit);
		}
		WeaponInstance->ApplyRecoilShotIfNeeded(ShotContext);
	}
}

void UFPSCombatGameplayAbilityRangedW::OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& DataHandle,
	FGameplayTag ApplicationTag)
{
	bHasTargetDataSent = true;
	UAbilitySystemComponent* ASC = CurrentActorInfo->AbilitySystemComponent.Get();
	check(ASC);

	FScopedPredictionWindow ScopedPredictionWindow(ASC);
	
	FGameplayAbilityTargetDataHandle LocalDataHandle(MoveTemp(const_cast<FGameplayAbilityTargetDataHandle&>(DataHandle)));
	
	const bool bShouldNotifyServer = CurrentActorInfo->IsLocallyControlled() && !CurrentActorInfo->IsNetAuthority();
	if (bShouldNotifyServer)
	{
		ASC->CallServerSetReplicatedTargetData(CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey(), LocalDataHandle, ApplicationTag, ASC->ScopedPredictionKey);
	}
	
	UFPSCombatRangedWeaponInstance* WeaponInstance = GetWeaponInstance();
	
	if (ASC && WeaponInstance && CurrentActorInfo->IsNetAuthority() && DamageEffectClass)
	{
		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass, GetAbilityLevel());
		SpecHandle.Data->SetSetByCallerMagnitude(FPSCombatGameplayTags::SetByCaller_Data_Damage,
		WeaponInstance->GetWeaponDefinition()->BaseDamage);
		for (auto It = LocalDataHandle.Data.CreateConstIterator(); It; ++It)
		{
			if (const FHitResult* HitResult = LocalDataHandle.Get(It.GetIndex())->GetHitResult())
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
	ASC->ConsumeClientReplicatedTargetData(CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey());
}

void UFPSCombatGameplayAbilityRangedW::OnTargetDataCancelledCallback()
{
	UAbilitySystemComponent* ASC = CurrentActorInfo->AbilitySystemComponent.Get();
	ASC->ConsumeClientReplicatedTargetData(CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey());
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

ECollisionChannel UFPSCombatGameplayAbilityRangedW::DetermineTraceChannel() const
{
	return FPSCombat_TraceChannel_Weapon;
}

void UFPSCombatGameplayAbilityRangedW::TryFireNextShot()
{
	UFPSCombatRangedWeaponInstance* WeaponData = GetWeaponInstance();
	if (WeaponData && WeaponData->WantsAnotherShot())
	{
		HandleFireInput();
	}
	else
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, CurrentActorInfo->IsNetAuthority(), false);
	}
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

	WeaponData->ConsumeRound();
	WeaponData->UpdateLastFireTime();
	WeaponData->NotifyShotHappens();

	
	if (WeaponData->WantsAnotherShot())
	{
		UAbilityTask_WaitDelay* WaitDelayTask = UAbilityTask_WaitDelay::WaitDelay(this, WeaponData->GetWeaponDefinition()->DurationBetweenShoot);
		WaitDelayTask->OnFinish.AddDynamic(this, &ThisClass::UFPSCombatGameplayAbilityRangedW::TryFireNextShot);
		WaitDelayTask->ReadyForActivation();
	}
	else
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, CurrentActorInfo->IsNetAuthority(), false);
	}
}