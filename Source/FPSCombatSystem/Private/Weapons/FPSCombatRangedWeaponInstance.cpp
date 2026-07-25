// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapons/FPSCombatRangedWeaponInstance.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "FPSCombatSystem/FPSCombatGameplayTags.h"
#include "Net/UnrealNetwork.h"
#include "Weapons/FPSCombatAmmoTypes.h"

bool UFPSCombatRangedWeaponInstance::CanReload() const
{
	return (ReserveAmmo>0 && CurrentAmmoInMag < GetWeaponDefinition()->ClipSize);
}

void UFPSCombatRangedWeaponInstance::ConsumeRound()
{
	APawn* Pawn = GetPawn();
	if (!Pawn || !Pawn->HasAuthority()) return;
	
	CurrentAmmoInMag = FMath::Max(0, CurrentAmmoInMag - 1);
	BroadcastAmmoChanged();
}

int32 UFPSCombatRangedWeaponInstance::ReloadAmmo()
{
	APawn* Pawn = GetPawn();
	UE_LOG(LogTemp, Warning, TEXT("ReloadAmmo called, Authority=%d"), Pawn ? Pawn->HasAuthority() : -1);
	if (!Pawn || !Pawn->HasAuthority()) return 0;

	const int32 NeededAmmo = GetWeaponDefinition()->ClipSize - CurrentAmmoInMag;
	const int32 Transferred = FMath::Min(NeededAmmo, ReserveAmmo);

	CurrentAmmoInMag += Transferred;
	ReserveAmmo -= Transferred;
	BroadcastAmmoChanged();

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

void UFPSCombatRangedWeaponInstance::BroadcastAmmoChanged() const
{
	APawn* Pawn = GetPawn();
	check(Pawn);


	FFPSCombatAmmoChangedMessage Message;
	Message.CurrentAmmo = CurrentAmmoInMag;
	Message.ReserveAmmo = ReserveAmmo;

	UGameplayMessageSubsystem::Get(this).BroadcastMessage(FPSCombatGameplayTags::Message_Ammo_Change, Message);
}

void UFPSCombatRangedWeaponInstance::ApplyRecoilForShot()
{
	const TArray<FVector2D>& RecoilPattern = GetWeaponDefinition()->RecoilPattern; 
	if (RecoilPattern.Num() == 0) return;

	const int32 Index = FMath::Clamp(CurrentBurstShotIndex, 0, RecoilPattern.Num() - 1);

	RecoilState.TargetRecoilOffset += RecoilPattern[Index];
}

FVector UFPSCombatRangedWeaponInstance::CalculateFireDirection(const FFPSCombatShotContext& ShotContext, FVector& AimDirection) const
{	
	
	if (ShotContext.bIsAiming && ShotContext.bStationary)
	{
		FRotator RecoilRotation = AimDirection.Rotation();
		RecoilRotation.Pitch -= RecoilState.TargetRecoilOffset.X;
		RecoilRotation.Yaw += RecoilState.TargetRecoilOffset.Y;
		return RecoilRotation.Vector();
	}

	if (ShotContext.bIsIdle && ShotContext.bStationary)
	{
		return AimDirection;
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

void UFPSCombatRangedWeaponInstance::RegisterShotFired(const FFPSCombatShotContext& ShotContext)
{
	if (ShotContext.bIsIdle)
	{
		CurrentBurstShotIndex = 0;
		RecoilState.TargetRecoilOffset = FVector2D::ZeroVector;
	}
	
	if (ShotContext.bIsAiming && ShotContext.bStationary)
	{
		ApplyRecoilForShot();
	}
	CurrentBurstShotIndex++;
	
	UpdateLastFireTime();
}

FFPSCombatShotContext UFPSCombatRangedWeaponInstance::MakeShotContext() const
{
	FFPSCombatShotContext Context;

	Context.bIsAiming = IsAiming();
	Context.bIsIdle = WasIdleBeforeThisShot();
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

void UFPSCombatRangedWeaponInstance::OnRep_CurrentAmmoInMag()
{
	UE_LOG(LogTemp, Warning, TEXT("OnRep_CurrentAmmoInMag fired, NewAmmo=%d"), CurrentAmmoInMag);
	BroadcastAmmoChanged();
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
		BroadcastAmmoChanged();
	}
	CurrentBurstShotIndex = 0;
	RecoilState = FFPSCombatRecoilState();
}

void UFPSCombatRangedWeaponInstance::OnUnequipped()
{
	Super::OnUnequipped();
}

void UFPSCombatRangedWeaponInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UFPSCombatRangedWeaponInstance, ReserveAmmo, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UFPSCombatRangedWeaponInstance, CurrentAmmoInMag, COND_OwnerOnly);
}