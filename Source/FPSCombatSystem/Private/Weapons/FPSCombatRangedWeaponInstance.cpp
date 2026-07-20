// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapons/FPSCombatRangedWeaponInstance.h"

#include "FPSCombatSystem/FPSCombatGameplayTags.h"
#include "Net/UnrealNetwork.h"

bool UFPSCombatRangedWeaponInstance::CanReload() const
{
	return (ReserveAmmo>0 && CurrentAmmoInMag < GetWeaponDefinition()->ClipSize);
}

void UFPSCombatRangedWeaponInstance::ConsumeRound()
{
	APawn* Pawn = GetPawn();
	if (!Pawn || !Pawn->HasAuthority()) return;


	CurrentAmmoInMag = FMath::Max(0, CurrentAmmoInMag-1);
	OnAmmoChanged.Broadcast(CurrentAmmoInMag, ReserveAmmo);
}

int32 UFPSCombatRangedWeaponInstance::ReloadAmmo()
{
	APawn* Pawn = GetPawn();
	if (!Pawn || !Pawn->HasAuthority()) return 0;

	const int32 NeededAmmo = GetWeaponDefinition()->ClipSize - CurrentAmmoInMag;
	const int32 Transferred = FMath::Min(NeededAmmo, ReserveAmmo);

	CurrentAmmoInMag += Transferred;
	ReserveAmmo -= Transferred;
	OnAmmoChanged.Broadcast(CurrentAmmoInMag, Transferred);

	return Transferred;
}

bool UFPSCombatRangedWeaponInstance::CanFire() const
{
	const double TimeSinceFired = GetTimeSinceLastFire();

	if (TimeSinceFired >= GetWeaponDefinition()->DurationBetweenShoot && HasAmmoInMag())
	{
		return true;
	}
	return false;
}

bool UFPSCombatRangedWeaponInstance::WasIdleBeforeThisShot() const
{
	return	GetTimeFromLastInteraction() > GetWeaponDefinition()->RecoilResetDelay;
}

bool UFPSCombatRangedWeaponInstance::IsAiming() const
{
	check(GetPawnASC())
	return GetPawnASC()->HasMatchingGameplayTag(FPSCombatGameplayTags::Weapon_Aiming);
}

void UFPSCombatRangedWeaponInstance::ApplyRecoilForShot(APlayerController* PC)
{
	if (!PC || !PC->IsLocalController()) return;

	const TArray<FVector2D>& RecoilPattern = GetWeaponDefinition()->RecoilPattern; 
	if (RecoilPattern.Num() == 0) return;

	const int32 Index = FMath::Clamp(CurrentBurstShotIndex, 0, RecoilPattern.Num() - 1);

	PC->AddPitchInput(-RecoilPattern[Index].X);
	PC->AddYawInput(RecoilPattern[Index].Y);
	
}

FVector UFPSCombatRangedWeaponInstance::CalculateFireDirection(const FFPSCombatShotContext& ShotContext, FVector& AimDirection) const
{	
	
	if (ShotContext.bIsAiming && ShotContext.bStationary)
	{
		return AimDirection;
	}

	if (ShotContext.bIsIdle)
	{
		return AimDirection;
	}

	float Spread = ShotContext.bIsAiming ? GetWeaponDefinition()->AimedMovingSpread : GetWeaponDefinition()->HipFireSpread;
	
	if (!ShotContext.bIsAiming && !ShotContext.bStationary)
	{
		Spread *= GetWeaponDefinition()->MovementHipFireMultiplier;
	}
	

	return FMath::VRandCone(AimDirection, FMath::DegreesToRadians(Spread));
		
}

void UFPSCombatRangedWeaponInstance::RegisterShotFired(const FFPSCombatShotContext& ShotContext, APlayerController* Controller)
{
	if (ShotContext.bIsIdle)
	{
		CurrentBurstShotIndex = 0;
	}

	if (ShotContext.bIsAiming && ShotContext.bStationary)
	{
		ApplyRecoilForShot(Controller);
	}
	CurrentBurstShotIndex++;
	
	UpdateLastFireTime();
}

FFPSCombatShotContext UFPSCombatRangedWeaponInstance::MakeShotContext() const
{
	FFPSCombatShotContext Context;

	Context.bIsAiming = IsAiming();
	Context.bIsIdle = WasIdleBeforeThisShot();
	Context.bStationary = IsPawnMoving();

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

void UFPSCombatRangedWeaponInstance::OnRep_CurrentAmmoInMag()
{
	OnAmmoChanged.Broadcast(CurrentAmmoInMag, ReserveAmmo);
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

	if (CurrentAmmoInMag < 0)
	{
		CurrentAmmoInMag = GetWeaponDefinition()->ClipSize;
		ReserveAmmo = GetWeaponDefinition()->ReserveAmmo;
	}
	CurrentBurstShotIndex = 0;
}

void UFPSCombatRangedWeaponInstance::OnUnequipped()
{
	Super::OnUnequipped();
}

void UFPSCombatRangedWeaponInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UFPSCombatRangedWeaponInstance, ReserveAmmo);
	DOREPLIFETIME(UFPSCombatRangedWeaponInstance, CurrentAmmoInMag);
	
}
