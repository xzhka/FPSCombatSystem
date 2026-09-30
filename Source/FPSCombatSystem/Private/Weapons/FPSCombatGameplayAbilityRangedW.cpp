// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/FPSCombatGameplayAbilityRangedW.h"

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

	const UAbilitySystemComponent* ASC = ActorInfo ? ActorInfo->AbilitySystemComponent.Get() : nullptr;
	const FGameplayAbilitySpec* Spec = ASC ? ASC->FindAbilitySpecFromHandle(Handle) : nullptr;
	const UFPSCombatRangedWeaponInstance* WeaponData = Spec ? Cast<UFPSCombatRangedWeaponInstance>(Spec->SourceObject.Get()) : nullptr;

	return WeaponData && WeaponData->CanFire();
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
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	bHasTargetDataSent = false;
	BindShotConfirmation(Handle, ActivationInfo.GetActivationPredictionKey());
	
	FireShot();
}

void UFPSCombatGameplayAbilityRangedW::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	TWeakObjectPtr<UFPSCombatRangedWeaponInstance> WeakInstanceData = GetWeaponInstance();

	const bool bShouldDriveReactivation = ActorInfo->IsLocallyControlled();
	const float NextActivationDelay = WeakInstanceData.IsValid() ? WeakInstanceData->GetWeaponDefinition()->DurationBetweenShoot : 0.f;
	const bool bOutOfAmmo = WeakInstanceData.IsValid() && !WeakInstanceData->HasAmmoInMag();
	FGameplayAbilitySpecHandle SpecHandleToReactivation = CurrentSpecHandle;
	AActor* Avatar = ActorInfo ? ActorInfo->AvatarActor.Get() : nullptr;
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (bOutOfAmmo && OnOutOfAmmoTag.IsValid() && Avatar)
	{
		FGameplayEventData Payload;
		Payload.EventTag = OnOutOfAmmoTag;

		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetAvatarActorFromActorInfo(), OnOutOfAmmoTag, Payload);
	}
	
	if (bWasCancelled)
	{
		if (WeakInstanceData.IsValid()) WeakInstanceData->AbortFireSequence();	
	}
	else if (bShouldDriveReactivation && WeakInstanceData.IsValid())
	{
		WeakInstanceData->ScheduleNextShotActivation(SpecHandleToReactivation, NextActivationDelay);
	}
}

void UFPSCombatGameplayAbilityRangedW::NotifyInputReleased(const FGameplayAbilitySpec& Spec)
{
	if (UFPSCombatRangedWeaponInstance* WeaponInstance = Cast<UFPSCombatRangedWeaponInstance>(Spec.SourceObject.Get()))
	{
		WeaponInstance->HandleInputReleased();
	}
}

void UFPSCombatGameplayAbilityRangedW::StartRangedWeaponTargeting(const FFPSCombatShotContext& Context)
{
	check(CurrentActorInfo);

	UAbilitySystemComponent* ASC = CurrentActorInfo->AbilitySystemComponent.Get();
	check(ASC);

	APawn* Pawn = Cast<APawn>(GetAvatarActorFromActorInfo());
	const bool bIsLocallyControlled = Pawn && Pawn->IsLocallyControlled();
	
	if (CurrentActorInfo->IsNetAuthority() && !bIsLocallyControlled) { return; }
	
	TArray<FHitResult> Hits;
	PerformLocalTargeting(Context, Hits);

	FGameplayAbilityTargetDataHandle TargetData;

	for (const FHitResult& FoundHitResult : Hits)
	{
		FGameplayAbilityTargetData_SingleTargetHit* TargetHit = new FGameplayAbilityTargetData_SingleTargetHit();
		TargetHit->HitResult = FoundHitResult;
		TargetData.Add(TargetHit);
	}

	OnShotTargetDataReady(TargetData, FGameplayTag(), CurrentActivationInfo.GetActivationPredictionKey());
}

void UFPSCombatGameplayAbilityRangedW::PerformLocalTargeting(const FFPSCombatShotContext& Context, TArray<FHitResult>& OutHits)
{
	UFPSCombatRangedWeaponInstance* WeaponInstance = GetWeaponInstance();
	APawn* Pawn = GetAvatarActorFromActorInfo() ? Cast<APawn>(GetAvatarActorFromActorInfo()) : nullptr;

	if (!Pawn || !WeaponInstance || !Pawn->IsLocallyControlled()) return;
	
	AController* Controller = Pawn->GetController();

	if (!Controller) return;

	FVector ViewLocation;
	FRotator ViewRotation;

	Controller->GetPlayerViewPoint(ViewLocation, ViewRotation);

	FVector AimDir = ViewRotation.Vector();
	const FVector ShotDir = WeaponInstance->CalculateFireDirection(Context, AimDir);

	const FVector EndPoint = ViewLocation + ShotDir * WeaponInstance->GetWeaponDefinition()->TraceRange;

	FCollisionQueryParams Params(SCENE_QUERY_STAT(WeaponTrace), true, Pawn);
	Params.bReturnPhysicalMaterial = true;
	Params.AddIgnoredActor(Pawn);

	const ECollisionChannel TraceChannel = DetermineTraceChannel();

	FHitResult Hit;
	if (!GetWorld()->LineTraceSingleByChannel(Hit, ViewLocation, EndPoint, TraceChannel, Params))
	{
		Hit = FHitResult();
		Hit.TraceStart = ViewLocation;
		Hit.TraceEnd = EndPoint;
		Hit.Location = EndPoint;
		Hit.ImpactPoint = EndPoint;
		Hit.bBlockingHit = false;
	}
	OutHits.Add(Hit);
	WeaponInstance->ApplyRecoilShotIfNeeded(Context);
}

void UFPSCombatGameplayAbilityRangedW::BindShotConfirmation(FGameplayAbilitySpecHandle Handle,
	FPredictionKey PredictionKey)
{
	UAbilitySystemComponent* ASC = CurrentActorInfo->AbilitySystemComponent.Get();
	check(ASC);

	FFPSInFlightShot& Shot = InFlightShots.AddDefaulted_GetRef();
	Shot.SpecHandle = Handle;
	Shot.PredictionKey = PredictionKey;

	Shot.DataReadyHandle = ASC->AbilityTargetDataSetDelegate(Handle, PredictionKey).AddUObject(this, &ThisClass::OnShotTargetDataReady, PredictionKey);
	Shot.DataCancelledHandle = ASC->AbilityTargetDataCancelledDelegate(Handle, PredictionKey).AddUObject(this, &ThisClass::OnShotTargetDataCancelled, PredictionKey);
}

void UFPSCombatGameplayAbilityRangedW::OnShotTargetDataReady(const FGameplayAbilityTargetDataHandle& DataHandle,
	FGameplayTag ApplicationTag, FPredictionKey ShotKey)
{
	int32 Index = InFlightShots.IndexOfByPredicate([ShotKey](const FFPSInFlightShot& S) { return S.PredictionKey == ShotKey; });
	if (Index == INDEX_NONE) return;

	const FFPSInFlightShot Shot = InFlightShots[Index];
	InFlightShots.RemoveAt(Index);
	
	bHasTargetDataSent = true;
	UAbilitySystemComponent* ASC = CurrentActorInfo->AbilitySystemComponent.Get();
	check(ASC);

	FScopedPredictionWindow ScopedPredictionWindow(ASC);
	
	FGameplayAbilityTargetDataHandle LocalDataHandle(MoveTemp(const_cast<FGameplayAbilityTargetDataHandle&>(DataHandle)));
	const bool bShouldNotifyServer = CurrentActorInfo->IsLocallyControlled() && !CurrentActorInfo->IsNetAuthority();
	if (bShouldNotifyServer)
	{
		ASC->CallServerSetReplicatedTargetData(Shot.SpecHandle, Shot.PredictionKey, LocalDataHandle, ApplicationTag, ASC->ScopedPredictionKey);
	}
	
	ProcessHitResult(LocalDataHandle);
	
	ASC->ConsumeClientReplicatedTargetData(Shot.SpecHandle, Shot.PredictionKey);
	ASC->AbilityTargetDataSetDelegate(Shot.SpecHandle, Shot.PredictionKey).Remove(Shot.DataReadyHandle);
	ASC->AbilityTargetDataCancelledDelegate(Shot.SpecHandle, Shot.PredictionKey).Remove(Shot.DataCancelledHandle);
}

void UFPSCombatGameplayAbilityRangedW::OnShotTargetDataCancelled(FPredictionKey ShotKey)
{
	int32 Index = InFlightShots.IndexOfByPredicate([ShotKey](const FFPSInFlightShot& S) { return S.PredictionKey == ShotKey; });
	if (Index == INDEX_NONE) return;

	const FFPSInFlightShot Shot = InFlightShots[Index];
	InFlightShots.RemoveAt(Index);

	if (UAbilitySystemComponent* ASC = CurrentActorInfo ? CurrentActorInfo->AbilitySystemComponent.Get() : nullptr)
	{
		ASC->ConsumeClientReplicatedTargetData(Shot.SpecHandle, Shot.PredictionKey);
		ASC->AbilityTargetDataSetDelegate(Shot.SpecHandle, Shot.PredictionKey).Remove(Shot.DataReadyHandle);
		ASC->AbilityTargetDataCancelledDelegate(Shot.SpecHandle, Shot.PredictionKey).Remove(Shot.DataCancelledHandle);
	}
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
	FRotator ViewRot;

	Controller->GetPlayerViewPoint(ViewLoc, ViewRot);

	const float MaxRange = FMath::Square(WeaponInstance->GetWeaponDefinition()->TraceRange + HitValidationRangeSlack);
	if (FVector::DistSquared(ViewLoc, HitResult.Location) > MaxRange)
	{
		return false;
	}
	if (!HitResult.bBlockingHit) { return true; }
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

void UFPSCombatGameplayAbilityRangedW::FireShot()
{
	UFPSCombatRangedWeaponInstance* WeaponData = GetWeaponInstance();
	
	WeaponData->ConsumeRound();
	WeaponData->UpdateLastFireTime();

	
	FFPSCombatShotContext ShotContext = WeaponData->NotifyShotFiredAndMakeShotContext();
	StartRangedWeaponTargeting(ShotContext);
	
	K2_OnShotFire();
}

void UFPSCombatGameplayAbilityRangedW::ProcessHitResult(const FGameplayAbilityTargetDataHandle& DataHandle)
{
	if (!CurrentActorInfo->IsNetAuthority()) return;

	InvalidateClientPredictionKey();
	
	UFPSCombatRangedWeaponInstance* WeaponInstance = GetWeaponInstance();
	UAbilitySystemComponent* ASC = CurrentActorInfo->AbilitySystemComponent.Get();
	if (!ASC || !WeaponInstance) return;
	
	UFPSCombatWeaponDefinition* WeaponDefinition = WeaponInstance->GetWeaponDefinition();
	if (!WeaponDefinition || !WeaponDefinition->DamageEffectClass) return;
	
	for (auto It = DataHandle.Data.CreateConstIterator(); It; ++It)
	{
		if (const FHitResult* HitResult = DataHandle.Get(It.GetIndex())->GetHitResult())
		{
			if (!IsHitResultValid(*HitResult)) continue;

			FGameplayEffectContextHandle CueContext = ASC->MakeEffectContext();
			CueContext.AddHitResult(*HitResult, true);
			
			if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitResult->GetActor()))
			{
				FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(WeaponDefinition->DamageEffectClass, GetAbilityLevel(), ASC->MakeEffectContext());
				if (SpecHandle.IsValid())
				{
					SpecHandle.Data->SetSetByCallerMagnitude(FPSCombatGameplayTags::SetByCaller_Data_Damage,
						WeaponInstance->GetWeaponDefinition()->BaseDamage);	
					
					ASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
				}
			}
			FGameplayCueParameters CueParams;
			CueParams.Location = HitResult->ImpactPoint;
			CueParams.Normal = HitResult->Normal;
			CueParams.PhysicalMaterial = HitResult->PhysMaterial;
			CueParams.Instigator =  ASC->GetAvatarActor();
			CueParams.EffectCauser = ASC->GetAvatarActor();
			CueParams.SourceObject = WeaponDefinition;
			CueParams.EffectContext = CueContext;

			if (HitResult->bBlockingHit && ImpactTag.IsValid())
			{
				ASC->ExecuteGameplayCue(ImpactTag, CueParams);
			}

			if (TracerTag.IsValid())
			{
				ASC->ExecuteGameplayCue(TracerTag, CueParams);
			}
		}
	}
}