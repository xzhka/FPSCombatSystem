// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/FPSCombatWeaponInstance.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "FPSCombatRangedWeaponInstance.generated.h"


USTRUCT()
struct FFPSCombatRecoilState
{
	GENERATED_BODY()
	FVector2D TargetRecoilOffset = FVector2D::ZeroVector;
};


struct FFPSCombatShotContext
{
	bool bIsAiming = false;
	bool bStationary = false;
	bool bIsIdle = false;
};

UCLASS(Blueprintable, BlueprintType)
class FPSCOMBATSYSTEM_API UFPSCombatRangedWeaponInstance : public UFPSCombatWeaponInstance
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintPure)
	FORCEINLINE int32 GetCurrentAmmo() const { return CurrentAmmoInMag; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE int32 GetReserveAmmo() const { return ReserveAmmo; }
	
	bool HasAmmoInMag() const { return CurrentAmmoInMag>0;}
	bool CanReload() const;
	void ConsumeRound();
	int32 ReloadAmmo();


	bool CanFire() const;

	void ApplyRecoilForShot();


	FVector CalculateFireDirection(const FFPSCombatShotContext& ShotContext, FVector& AimDirection) const;
	
	void RegisterShotFired(const FFPSCombatShotContext& ShotContext);

	FFPSCombatShotContext MakeShotContext() const;

	UFPSCombatWeaponDefinition* GetWeaponDefinition() const;
	
protected:
	
	UFUNCTION()
	void OnRep_CurrentAmmoInMag();

	UAbilitySystemComponent* GetPawnASC() const;
	
	bool IsPawnMoving() const;
	bool WasIdleBeforeThisShot() const;
	bool IsAiming() const;

private:

	UPROPERTY(ReplicatedUsing = OnRep_CurrentAmmoInMag)
	int32 CurrentAmmoInMag = -1;

	UPROPERTY(Replicated)
	int32 ReserveAmmo = -1;
	
	void BroadcastAmmoChanged() const;

	
	int32 CurrentBurstShotIndex = 0;
	
	FFPSCombatRecoilState RecoilState;
	
public:
	virtual void OnEquipped() override;
	virtual void OnUnequipped() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
