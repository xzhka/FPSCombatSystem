// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/FPSCombatGameplayAbilityRangedW.h"

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
	if (!WeaponData && !WeaponData->CanFire())
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}

	StartRangedWeaponTargeting();

	
	WeaponData->UpdateLastFireTime();
	
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UFPSCombatGameplayAbilityRangedW::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UFPSCombatGameplayAbilityRangedW::StartRangedWeaponTargeting()
{
	UAbilitySystemComponent* ASC = CurrentActorInfo->AbilitySystemComponent.Get();
	check(ASC);

	ASC->AbilityTargetDataSetDelegate(CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey()).AddUObject(this, &ThisClass::OnTargetDataReadyCallback);

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
	
	FHitResult Hit;
	if (GetWorld()->LineTraceSingleByChannel(Hit, LocDir, EndDir, ECollisionChannel::ECC_Visibility, Params))
	{
		OutHits.Add(Hit);
	}
	WeaponInstance->RegisterShotFired(ShotContext, Cast<APlayerController>(Controller));
}

void UFPSCombatGameplayAbilityRangedW::OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& DataHandle,
	FGameplayTag ApplicationTag)
{
	UAbilitySystemComponent* ASC = CurrentActorInfo->AbilitySystemComponent.Get();
	if (!ASC || !CurrentActorInfo->IsNetAuthority()) return;

	UFPSCombatRangedWeaponInstance* WeaponInstance = GetWeaponInstance();
	if (!WeaponInstance) return;

	WeaponInstance->ConsumeRound();

	if(DamageEffectClass)
	{
		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass->GetClass(), GetAbilityLevel());
		SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("SetByCaller.Data.Damage"),
			WeaponInstance->GetWeaponDefinition()->BaseDamage);
		for (auto It = DataHandle.Data.CreateConstIterator(); It; ++It)
		{
			if (const FHitResult* HitResult = DataHandle.Get(It.GetIndex())->GetHitResult())
			{
				if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitResult->GetActor()))
				{
					ASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
				}
			}
		}
	}
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
