// FPS Combat project

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "FPSCombatDamageExecCalc.generated.h"

/** UFPSCombatDamageExecCalc
 * 
 *  Execution used for effects to deal damage to health attribute
 */
UCLASS()
class FPSCOMBATSYSTEM_API UFPSCombatDamageExecCalc : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	UFPSCombatDamageExecCalc();

protected:
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
