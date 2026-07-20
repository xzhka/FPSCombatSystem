// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/FPSCombatWeaponInstance.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "FPSCombatRangedWeaponInstance.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnAmmoChanged, int32, int32);


struct FFPSCombatShotContext
{
	bool bIsAiming = false;
	bool bStationary = false;
	bool bIsIdle = false;
};


UCLASS()
class FPSCOMBATSYSTEM_API UFPSCombatRangedWeaponInstance : public UFPSCombatWeaponInstance
{
	GENERATED_BODY()

public:

	UPROPERTY(ReplicatedUsing = OnRep_CurrentAmmoInMag)
	int32 CurrentAmmoInMag = -1;

	UPROPERTY(Replicated)
	int32 ReserveAmmo = -1;

	int32 CurrentBurstShotIndex = -1;
	
	bool HasAmmoInMag() const { return CurrentAmmoInMag>0;}
	bool CanReload() const;
	void ConsumeRound();
	int32 ReloadAmmo();


	bool CanFire() const;

	void ApplyRecoilForShot(APlayerController* PC);


	FVector CalculateFireDirection(const FFPSCombatShotContext& ShotContext, FVector& AimDirection) const;
	
	void RegisterShotFired(const FFPSCombatShotContext& ShotContext, APlayerController* Controller);

	FFPSCombatShotContext MakeShotContext() const;

	UFPSCombatWeaponDefinition* GetWeaponDefinition() const;
	
protected:
	
	FOnAmmoChanged OnAmmoChanged;
	
	UFUNCTION()
	void OnRep_CurrentAmmoInMag();

	UAbilitySystemComponent* GetPawnASC() const;
	
	bool IsPawnMoving() const;
	bool WasIdleBeforeThisShot() const;
	bool IsAiming() const;
	
public:
	virtual void OnEquipped() override;
	virtual void OnUnequipped() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
