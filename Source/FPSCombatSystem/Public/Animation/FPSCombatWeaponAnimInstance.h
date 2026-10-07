// FPS Combat project

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "FPSCombatWeaponAnimInstance.generated.h"

/** UFPSCombatWeaponAnimInstance
 *
 * Animation instance represented
 * weapon animation class
 */
UCLASS()
class FPSCOMBATSYSTEM_API UFPSCombatWeaponAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Weapon|Mag")
	void SetMagInHand(bool bHasInHand) { bMagInHand = bHasInHand; }

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Alpha")
	float MagAlpha = 1.f;
	
	UPROPERTY(BlueprintReadOnly, Category = "Alpha")
	bool bMagInHand = false;

	UPROPERTY(BlueprintReadOnly, Category = "Alpha")
	FTransform MagTarget = FTransform();

	UPROPERTY(EditDefaultsOnly, Category = "Name")
	FName MagGripSocketName = "MagGrip";

	UPROPERTY(EditDefaultsOnly, Category = "Name")
	FName WeaponMagSocketName = "Magazine";

	UPROPERTY(EditDefaultsOnly, Category = "Speed")
	float MagBlendSpeed = 15.f;
};
