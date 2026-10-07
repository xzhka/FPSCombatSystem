// FPS Combat project

#pragma once

#include "CoreMinimal.h"
#include "Equipment/FPSCombatEquipmentInstance.h"
#include "Weapons/FPSCombatThrowableDefinition.h"
#include "FPSCombatThrowableInstance.generated.h"

/** UFPSCombatThrowableInstance
 * 
 *  A part of equipment instance which represents the throwable objects
 */
UCLASS(Blueprintable, BlueprintType)
class FPSCOMBATSYSTEM_API UFPSCombatThrowableInstance : public UFPSCombatEquipmentInstance
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	UFPSCombatThrowableDefinition* GetThrowableDefinition() const ;

	UFUNCTION(BlueprintCallable)
	bool HasChargesRemaining() const;

	UFUNCTION(BlueprintCallable)
	void ConsumeProjectile(FGameplayTag ProjectileTag) const;
};
