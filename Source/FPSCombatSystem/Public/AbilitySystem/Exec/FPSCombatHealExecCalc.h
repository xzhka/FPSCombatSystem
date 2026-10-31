// FPS Combat project
#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "FPSCombatHealExecCalc.generated.h"

/** UFPSCombatHealExecCalc
 * 
 *  Execution used for effects to deal heal to health attribute
 */
UCLASS()
class FPSCOMBATSYSTEM_API UFPSCombatHealExecCalc : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	UFPSCombatHealExecCalc();

protected:
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
