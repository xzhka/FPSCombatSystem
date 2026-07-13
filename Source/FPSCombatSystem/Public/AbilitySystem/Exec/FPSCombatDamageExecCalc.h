// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "FPSCombatDamageExecCalc.generated.h"

/**
 * 
 */
UCLASS()
class FPSCOMBATSYSTEM_API UFPSCombatDamageExecCalc : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	UFPSCombatDamageExecCalc();
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
