// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapons/FPSCombatRangedWeaponInstance.h"

#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "FPSCombatSystem/FPSCombatGameplayTags.h"
#include "Net/UnrealNetwork.h"

bool UFPSCombatRangedWeaponInstance::CanReload() const
{
	const UFPSCombatItemInstance* Item = GetItemInstance();
	if (!Item) return false;
	return Item->GetStack(FPSCombatGameplayTags::Data_Weapon_SpareAmmo) > 0
		&& Item->GetStack(FPSCombatGameplayTags::Data_Weapon_Ammo_Mag) < GetWeaponDefinition()->ClipSize;
}

int32 UFPSCombatRangedWeaponInstance::AddReserveAmmo(int32 Amount)
{
	UFPSCombatItemInstance* Item = GetItemInstance();
	AActor* Outer = GetTypedOuter<AActor>();
	if (!Item || !Outer || !Outer->HasAuthority() || Amount <= 0)
	{
		return 0;
	}

	const int32 MaxReserveAmmo = GetWeaponDefinition()->ReserveAmmo;
	const int32 OldReserveAmmo = Item->GetStack(FPSCombatGameplayTags::Data_Weapon_SpareAmmo);
	const int32 DeltaAmmo = FMath::Clamp(OldReserveAmmo + Amount, 0, MaxReserveAmmo) - OldReserveAmmo;

	if (DeltaAmmo > 0)
	{
		Item->AddStackCount(FPSCombatGameplayTags::Data_Weapon_SpareAmmo, DeltaAmmo);
	}

	return DeltaAmmo;
}

void UFPSCombatRangedWeaponInstance::ConsumeRound()
{
	const AActor* Outer = GetTypedOuter<AActor>();
	UFPSCombatItemInstance* ItemInstance = GetItemInstance();
	
	if (!ItemInstance || !Outer || !Outer->HasAuthority()) return;
	
	if (HasAmmoInMag())
	{
		ItemInstance->RemoveStackCount(FPSCombatGameplayTags::Data_Weapon_Ammo_Mag, 1);
	}
}

int32 UFPSCombatRangedWeaponInstance::ReloadAmmo()
{
	const AActor* Outer = GetTypedOuter<AActor>();
	UFPSCombatItemInstance* ItemInstance = GetItemInstance();
	
	if (!ItemInstance || !Outer || !Outer->HasAuthority()) return 0;

	const int32 NeededAmmo = GetWeaponDefinition()->ClipSize - GetCurrentAmmo();
	const int32 Transferred = FMath::Min(NeededAmmo, GetReserveAmmo());

	if (Transferred > 0)
	{
		ItemInstance->AddStackCount(FPSCombatGameplayTags::Data_Weapon_Ammo_Mag, Transferred);
		ItemInstance->RemoveStackCount(FPSCombatGameplayTags::Data_Weapon_SpareAmmo, Transferred);
	}
	return Transferred;
}

void UFPSCombatRangedWeaponInstance::AbortFireSequence()
{
	CancelScheduledActivation();
	bIsContinuation = false;
	bReactivationPending = false;
	CurrentRecoilShotIndex = 0;
	RecoilState = FFPSCombatRecoilState();
	if (UFPSCombatFireMode* FireM = GetFireMode())
	{
		FireM->ResetSequence();
	}
}

bool UFPSCombatRangedWeaponInstance::ConsumeIsContinuation()
{
	const bool bWas = bIsContinuation;
	bIsContinuation = false;
	return bWas;
}

bool UFPSCombatRangedWeaponInstance::WantsAnotherShotThisActivation() const
{
	UFPSCombatFireMode* FireM = GetFireMode();
	return FireM && HasAmmoInMag() && FireM->WantsAnotherShotThisActivation(this);
}

void UFPSCombatRangedWeaponInstance::ScheduleNextShotActivation(const FGameplayAbilitySpecHandle& SpecHandle,
	float Delay)
{
	UFPSCombatFireMode* FireM = GetFireMode();
	if (!FireM) return;

	const bool bBurstWantsNextShot = WantsAnotherShotThisActivation();
	const bool bIsFullAuto = FireM->GetActivationPolicy() == EFPSCombatAbilityActivationPolicy::WhileInputActive;

	if (!bBurstWantsNextShot && !bIsFullAuto)
	{
		bReactivationPending = false;
		return;
	}

	
	APawn* Pawn = GetPawn();
	UAbilitySystemComponent* ASC = GetPawnASC();
	if (!Pawn || !ASC) { AbortFireSequence(); return; }

	bReactivationPending = true;
	
	const TWeakObjectPtr<UAbilitySystemComponent> WeakASC = ASC;
	const TWeakObjectPtr<UFPSCombatRangedWeaponInstance> WeakInstance = this;

	Pawn->GetWorldTimerManager().SetTimer(NextActivationTimerHandle, FTimerDelegate::CreateWeakLambda(this,
		[WeakASC, WeakInstance,SpecHandle, bBurstWantsNextShot]()
	{
		UAbilitySystemComponent* StrongASC = WeakASC.Get();
		UFPSCombatRangedWeaponInstance* StrongInstance = WeakInstance.Get();
		if (!StrongASC || !StrongInstance) return;

		const FGameplayAbilitySpec* Spec = StrongASC->FindAbilitySpecFromHandle(SpecHandle);
		const bool bStillWantsToFire = bBurstWantsNextShot || (Spec && Spec->InputPressed);
			
		if (!bStillWantsToFire)
		{
			StrongInstance->AbortFireSequence();
			return;
		}

		StrongInstance->bReactivationPending = false;
		StrongInstance->bIsContinuation = true;	

		if (!StrongASC->TryActivateAbility(SpecHandle))
		{
			StrongInstance->AbortFireSequence();
		}
	}), Delay, false);
	
}

void UFPSCombatRangedWeaponInstance::CancelScheduledActivation()
{
	if (APawn* Pawn = GetPawn())
	{
		Pawn->GetWorldTimerManager().ClearTimer(NextActivationTimerHandle);
	}
}

bool UFPSCombatRangedWeaponInstance::CanFire() const
{
	if (bReactivationPending) return false;
	const double TimeSinceFired = GetTimeSinceLastFire();

	if (TimeSinceFired >= GetWeaponDefinition()->DurationBetweenShoot && HasAmmoInMag())
	{
		return true;
	}
	return false;
}

bool UFPSCombatRangedWeaponInstance::WasIdleBeforeThisShot() const
{
	return GetTimeFromLastInteraction() > GetWeaponDefinition()->RecoilResetDelay;
}

bool UFPSCombatRangedWeaponInstance::IsAiming() const
{
	check(GetPawnASC())
	return GetPawnASC()->HasMatchingGameplayTag(FPSCombatGameplayTags::Weapon_Aiming);
}

void UFPSCombatRangedWeaponInstance::OnFireMontageAdd()
{
	if (UFPSCombatWeaponDefinition* Def = GetWeaponDefinition())
	{
		CachedFireMontage = Def->FireMontage.Get();
	}
}

void UFPSCombatRangedWeaponInstance::ApplyRecoilForShot()
{
	const TArray<FVector2D>& RecoilPattern = GetWeaponDefinition()->RecoilPattern; 
	if (RecoilPattern.Num() == 0) return;

	if (CurrentRecoilShotIndex >= RecoilPattern.Num())
	{
		return;
	}

	RecoilState.TargetRecoilOffset += RecoilPattern[CurrentRecoilShotIndex];
}

int32 UFPSCombatRangedWeaponInstance::GetCurrentAmmo() const
{
	const UFPSCombatItemInstance* ItemInstance = GetItemInstance();

	return ItemInstance ? ItemInstance->GetStack(FPSCombatGameplayTags::Data_Weapon_Ammo_Mag) : 0;
}

int32 UFPSCombatRangedWeaponInstance::GetReserveAmmo() const
{
	const UFPSCombatItemInstance* ItemInstance = GetItemInstance();

	return ItemInstance ? ItemInstance->GetStack(FPSCombatGameplayTags::Data_Weapon_SpareAmmo) : 0;
}

bool UFPSCombatRangedWeaponInstance::HasAmmoInMag() const
{
	const UFPSCombatItemInstance* ItemInstance = GetItemInstance();

	return ItemInstance ? ItemInstance->GetStack(FPSCombatGameplayTags::Data_Weapon_Ammo_Mag) > 0 : 0;
}

UFPSCombatFireMode* UFPSCombatRangedWeaponInstance::GetFireMode() const
{
	if (!FireMode && GetWeaponDefinition() && GetWeaponDefinition()->FireModeClass)
	{
		const_cast<UFPSCombatRangedWeaponInstance*>(this)->FireMode = NewObject<UFPSCombatFireMode>(const_cast<UFPSCombatRangedWeaponInstance*>(this), GetWeaponDefinition()->FireModeClass);
	}
	return FireMode;
}

FVector UFPSCombatRangedWeaponInstance::CalculateFireDirection(const FFPSCombatShotContext& ShotContext, FVector& AimDirection) const
{	
	if (ShotContext.bIsFreshSequence && ShotContext.bStationary)
	{
		return AimDirection;
	}
	
	if (ShotContext.bIsAiming && ShotContext.bStationary)
	{
		FRotator RecoilRotation = AimDirection.Rotation();
		RecoilRotation.Pitch -= RecoilState.TargetRecoilOffset.X;
		RecoilRotation.Yaw += RecoilState.TargetRecoilOffset.Y;
		return RecoilRotation.Vector();
	}

	float Spread = ShotContext.bIsAiming ? GetWeaponDefinition()->AimedMovingSpread : GetWeaponDefinition()->HipFireSpread;
	if (!ShotContext.bIsAiming && !ShotContext.bStationary)
	{
		Spread *= GetWeaponDefinition()->MovementHipFireMultiplier;
	}
	Spread = FMath::Min(Spread, GetWeaponDefinition()->MaxSpreadDegrees);

	const float Azimuth = FMath::FRandRange(0.f, 360.f);
	const float T = FMath::FRand();
	const float Radius = Spread * FMath::Pow(T, GetWeaponDefinition()->SpreadBiasExponent);
	
	FRotator SpreadRotation = AimDirection.Rotation();

	SpreadRotation.Pitch += Radius * FMath::Sin(FMath::DegreesToRadians(Azimuth));
	SpreadRotation.Yaw += Radius * FMath::Cos(FMath::DegreesToRadians(Azimuth));
	
	return SpreadRotation.Vector();
		
}

void UFPSCombatRangedWeaponInstance::ApplyRecoilShotIfNeeded(const FFPSCombatShotContext& ShotContext)
{
	if (ShotContext.bIsAiming && ShotContext.bStationary)
	{
		ApplyRecoilForShot();
		CurrentRecoilShotIndex++;
	}
}

FFPSCombatShotContext UFPSCombatRangedWeaponInstance::NotifyShotFiredAndMakeShotContext()
{

	const bool bFreshSequence = !ConsumeIsContinuation();
	if (bFreshSequence)
	{
		HandleInputPressed();
		CurrentRecoilShotIndex = 0;
		RecoilState.TargetRecoilOffset = FVector2D::ZeroVector;
	}

	if (UFPSCombatFireMode* FireM = GetFireMode())
	{
		FireM->NotifyFireShot(this);
	}
	
	FFPSCombatShotContext Context;
	Context.bIsAiming = IsAiming();
	Context.bIsFreshSequence = bFreshSequence;
	Context.bStationary = !IsPawnMoving();
	
	return Context;
}

UFPSCombatWeaponDefinition* UFPSCombatRangedWeaponInstance::GetWeaponDefinition() const
{
	check(GetDefinition());
	return Cast<UFPSCombatWeaponDefinition>(GetDefinition());
}

bool UFPSCombatRangedWeaponInstance::IsPawnMoving() const
{
	check(GetPawnASC());
	UAbilitySystemComponent* ASC = GetPawnASC();
	return ASC->HasMatchingGameplayTag(FPSCombatGameplayTags::State_Moving_Walking);
}

UAbilitySystemComponent* UFPSCombatRangedWeaponInstance::GetPawnASC() const
{
	APawn* Pawn = GetPawn();
	if (!Pawn) return nullptr;

	if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Pawn))
	{
		return ASC;
	}
	return nullptr;
}

void UFPSCombatRangedWeaponInstance::OnEquipped()
{
	Super::OnEquipped();

	if (!FireMode && GetWeaponDefinition()->FireModeClass)
	{
		FireMode = NewObject<UFPSCombatFireMode>(this, GetWeaponDefinition()->FireModeClass);
	}

	if (UFPSCombatWeaponDefinition* WeaponDef = GetWeaponDefinition())
	{
		if (!WeaponDef->FireMontage.IsNull())
		{
			FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
			Streamable.RequestAsyncLoad(WeaponDef->FireMontage.ToSoftObjectPath(), FStreamableDelegate::CreateUObject(this, &UFPSCombatRangedWeaponInstance::OnFireMontageAdd));
		}
	}

	if (UFPSCombatItemInstance* Item = GetItemInstance())
	{
		AActor* Outer = GetTypedOuter<AActor>();
		if (Outer && Outer->HasAuthority() && !Item->HasStatTag(FPSCombatGameplayTags::Data_Weapon_Ammo_Mag))
		{
			Item->AddStackCount(FPSCombatGameplayTags::Data_Weapon_Ammo_Mag, GetWeaponDefinition()->ClipSize);
			Item->AddStackCount(FPSCombatGameplayTags::Data_Weapon_SpareAmmo, GetWeaponDefinition()->ReserveAmmo);
		}
	}
	CurrentRecoilShotIndex = 0;
	RecoilState = FFPSCombatRecoilState();
}

void UFPSCombatRangedWeaponInstance::OnUnequipped()
{
	CachedFireMontage = nullptr;
	AbortFireSequence();
	Super::OnUnequipped();
}