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
	if (!Pawn) return;
	
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
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("ScheduleNextShotActivation. bWantsNextShot: %d, bIsFullAuto: %d"), bBurstWantsNextShot, bIsFullAuto);

	
	APawn* Pawn = GetPawn();
	UAbilitySystemComponent* ASC = GetPawnASC();
	if (!Pawn || !ASC) return;

	SequenceState = EFPSCombatFireSequenceState::PendingReactivation;

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
		
		if (!bStillWantsToFire || !StrongASC->TryActivateAbility(SpecHandle))
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

bool UFPSCombatRangedWeaponInstance::ConsumeContinuationFlag()
{
	const bool bWasContinuation = (SequenceState == EFPSCombatFireSequenceState::PendingReactivation);
	SequenceState = EFPSCombatFireSequenceState::Idle;
	return bWasContinuation;
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
	return GetTimeFromLastInteraction() > GetWeaponDefinition()->RecoilResetDelay;
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

void UFPSCombatRangedWeaponInstance::AbortFireSequence()
{
	CancelScheduledActivation();
	SequenceState = EFPSCombatFireSequenceState::Idle;
	if (UFPSCombatFireMode* FireM = GetFireMode())
	{
		FireM->ResetSequence();
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
		UE_LOG(LogTemp, Warning, TEXT("CalculateFireDirection. Idle, Stationary"));
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

FFPSCombatShotContext UFPSCombatRangedWeaponInstance::MakeShotContext()
{
	FFPSCombatShotContext Context;

	Context.bIsAiming = IsAiming();
	Context.bIsFreshSequence = !ConsumeContinuationFlag();
	Context.bStationary = !IsPawnMoving();
	
	if (Context.bIsFreshSequence)
	{
		CurrentRecoilShotIndex = 0;
		RecoilState.TargetRecoilOffset = FVector2D::ZeroVector;
	}
	
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

	if (!FireMode && GetWeaponDefinition()->FireModeClass)
	{
		FireMode = NewObject<UFPSCombatFireMode>(this, GetWeaponDefinition()->FireModeClass);
	}
	
	if (CurrentAmmoInMag < 0)
	{
		CurrentAmmoInMag = GetWeaponDefinition()->ClipSize;
		ReserveAmmo = GetWeaponDefinition()->ReserveAmmo;
		BroadcastAmmoChanged();
	}
	CurrentRecoilShotIndex = 0;
	RecoilState = FFPSCombatRecoilState();
}

void UFPSCombatRangedWeaponInstance::OnUnequipped()
{
	CancelScheduledActivation();
	Super::OnUnequipped();
}

void UFPSCombatRangedWeaponInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UFPSCombatRangedWeaponInstance, ReserveAmmo, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UFPSCombatRangedWeaponInstance, CurrentAmmoInMag, COND_OwnerOnly);
}