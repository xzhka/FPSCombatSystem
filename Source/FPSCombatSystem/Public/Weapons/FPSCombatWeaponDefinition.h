// FPS Combat project

#pragma once

#include "CoreMinimal.h"
#include "Equipment/FPSCombatEquipmentDefinition.h"
#include "FPSCombatWeaponDefinition.generated.h"

class UFPSCombatFireMode;


/** UFPSCombatWeaponDefinition
 *
 *  Stats definition for every weapon instance
 */
UCLASS(Blueprintable, BlueprintType)
class FPSCOMBATSYSTEM_API UFPSCombatWeaponDefinition : public UFPSCombatEquipmentDefinition
{
	GENERATED_BODY()

public:
	
	/** Damage */
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float BaseDamage = 20.f;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TSubclassOf<UGameplayEffect> DamageEffectClass;
	
	/** Fire characteristics */
	
	UPROPERTY(EditDefaultsOnly, Category = "Fire")
	TSubclassOf<UFPSCombatFireMode> FireModeClass;

	/** Distance the hit trace travels. */
	UPROPERTY(EditDefaultsOnly, Category = "Fire", meta = (ClampMin = "0", Units = "cm"))
	float TraceRange = 10000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Fire|Spread", meta = (ClampMin = "0", Units = "Degrees"))
	float HipFireSpread = 12.f;

	UPROPERTY(EditDefaultsOnly, Category = "Fire|Spread", meta = (ClampMin = "0", Units = "Degrees"))
	float AimedMovingSpread = 8.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Fire|Spread", meta = (ClampMin = "0", Units = "Degrees"))
	float MaxSpreadDegrees = 40.f;

	/** Time between shots (1 / fire rate). */
	UPROPERTY(EditDefaultsOnly, Category = "Fire", meta = (ClampMin = "0", Units = "Seconds"))
	float DurationBetweenShoot = 0.2f;

	/** Shot distribution inside the cone: radius = Spread * T^Exponent. 0.5 = uniform, higher = tighter to center. */
	UPROPERTY(EditDefaultsOnly, Category = "Fire|Spread", meta = (ClampMin = "0.1"))
	float SpreadBiasExponent = 1.5f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Fire|Recoil")
	TArray<FVector2D> RecoilPattern;
	
	UPROPERTY(EditDefaultsOnly, Category = "Fire|Recoil", meta = (ClampMin = "0", Units = "Seconds"))
	float RecoilResetDelay = 0.3f;

	UPROPERTY(EditDefaultsOnly, Category = "Fire|Spread", meta = (ClampMin = "1"))
	float MovementHipFireMultiplier = 1.5f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Reload")
	float ReloadDuration = 2.f;

	/** Animation */
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	FName LeftHandGripSocket;
	
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TMap<FGameplayTag, TSoftObjectPtr<UAnimMontage>> ActionAnimations;
};