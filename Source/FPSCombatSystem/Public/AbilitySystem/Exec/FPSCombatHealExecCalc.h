// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "FPSCombatHealExecCalc.generated.h"

/**
 * 
 */
UCLASS()
class FPSCOMBATSYSTEM_API UFPSCombatHealExecCalc : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	UFPSCombatHealExecCalc();
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
