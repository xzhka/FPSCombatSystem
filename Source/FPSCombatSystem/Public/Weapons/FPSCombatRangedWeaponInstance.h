// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/FPSCombatWeaponInstance.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "FPSCombatWeaponDefinition.h"
#include "Fire/FPSCombatFireMode.h"
#include "FPSCombatSystem/FPSCombatMessageTypes.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Items/FPSCombatItemInstance.h"
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
	bool bIsFreshSequence = false;
};

UCLASS(Blueprintable, BlueprintType)
class FPSCOMBATSYSTEM_API UFPSCombatRangedWeaponInstance : public UFPSCombatWeaponInstance
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE int32 GetCurrentAmmo() const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE int32 GetReserveAmmo() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Weapon")
	FORCEINLINE UAnimMontage* GetAnimMontage() { return CachedFireMontage; }
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UFPSCombatFireMode* GetFireMode() const;
		
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UFPSCombatWeaponDefinition* GetWeaponDefinition() const;
	
	bool HasAmmoInMag() const;
	bool CanReload() const;
	int32 AddReserveAmmo(int32 Amount);
	void ConsumeRound();
	int32 ReloadAmmo();

	void HandleInputPressed() { if (UFPSCombatFireMode* FireM = GetFireMode()) FireM->OnInputPressed(this); }
	void HandleInputReleased() { if (UFPSCombatFireMode* FireM = GetFireMode()) FireM->OnInputReleased(this); }

	void AbortFireSequence();
	bool ConsumeIsContinuation();

	bool WantsAnotherShotThisActivation() const;
	
	void ScheduleNextShotActivation(const FGameplayAbilitySpecHandle& SpecHandle, float Delay);
	void CancelScheduledActivation();

	bool CanFire() const;
	void ApplyRecoilForShot();
	FVector CalculateFireDirection(const FFPSCombatShotContext& ShotContext, FVector& AimDirection) const;
	
	void ApplyRecoilShotIfNeeded(const FFPSCombatShotContext& ShotContext);

	FFPSCombatShotContext NotifyShotFiredAndMakeShotContext();
protected:
	
	
	UAbilitySystemComponent* GetPawnASC() const;
	
	bool IsPawnMoving() const;
	bool WasIdleBeforeThisShot() const;
	bool IsAiming() const;

private:
	void OnFireMontageAdd();
	
	UPROPERTY(Transient)
	TObjectPtr<UFPSCombatFireMode> FireMode;

	UPROPERTY(Transient)
	TObjectPtr<UAnimMontage> CachedFireMontage;
	
	int32 CurrentRecoilShotIndex = 0;

	bool bIsContinuation = false;

	bool bReactivationPending = false;
	
	FFPSCombatRecoilState RecoilState;
	
	FTimerHandle NextActivationTimerHandle;
	
public:
	virtual void OnEquipped() override;
	virtual void OnUnequipped() override;
};
