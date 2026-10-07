// FPS Combat project

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/FPSCombatAbilityTypes.h"
#include "FPSCombatFireMode.generated.h"

class UFPSCombatRangedWeaponInstance;
/** UFPSCombatFireMode
 *
 *	An object which represents the weapon fire mode
 */
UCLASS(Abstract, Blueprintable, DefaultToInstanced)
class FPSCOMBATSYSTEM_API UFPSCombatFireMode : public UObject
{
	GENERATED_BODY()

public:
	virtual void OnInputPressed(UFPSCombatRangedWeaponInstance* Instance) {} 

	virtual void OnInputReleased(UFPSCombatRangedWeaponInstance* Instance) {}

	virtual void NotifyFireShot(UFPSCombatRangedWeaponInstance* Instance) {}

	virtual bool WantsAnotherShotThisActivation(const UFPSCombatRangedWeaponInstance* Instance) const { return false; }

	virtual void ResetSequence() {};
	
	virtual EFPSCombatAbilityActivationPolicy GetActivationPolicy() const
	{
		return EFPSCombatAbilityActivationPolicy::OnInputTriggered;
	}
};

/** UFPSCombatFireMode_Burst
 *
 *	Derived class for burst fire mode weapons
 */
UCLASS()
class UFPSCombatFireMode_Burst : public UFPSCombatFireMode
{
	GENERATED_BODY()

public:
	virtual void OnInputPressed(UFPSCombatRangedWeaponInstance* Instance) override { CurrentBurst=0; }
	virtual void NotifyFireShot(UFPSCombatRangedWeaponInstance* Instance) override { ++CurrentBurst; }
	virtual bool WantsAnotherShotThisActivation(const UFPSCombatRangedWeaponInstance* Instance) const override { return CurrentBurst < BurstSize; }
	virtual void ResetSequence() override { CurrentBurst = 0; }

	
private:
	UPROPERTY(EditDefaultsOnly, Category = "Fire")
	int32 BurstSize = 3;
	
	int32 CurrentBurst = 0;
};

/** UFPSCombatFireMode_Semi
 *
 *	Derived class for semi fire mode weapons
 */
UCLASS()
class UFPSCombatFireMode_Semi : public UFPSCombatFireMode
{
	GENERATED_BODY()
};

/** UFPSCombatFireMode_FullAuto
 *
 *	Derived class for auto shooting mode weapons
 */
UCLASS()
class UFPSCombatFireMode_FullAuto : public UFPSCombatFireMode
{
	GENERATED_BODY()

public:
	virtual EFPSCombatAbilityActivationPolicy GetActivationPolicy() const override { return EFPSCombatAbilityActivationPolicy::WhileInputActive; }
};
