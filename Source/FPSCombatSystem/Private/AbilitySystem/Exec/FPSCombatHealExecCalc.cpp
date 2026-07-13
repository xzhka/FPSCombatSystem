// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Exec/FPSCombatHealExecCalc.h"

#include "AbilitySystem/Attributes/FPSCombatAttributeSet.h"

struct FFPSCombatHealStatics
{
	FGameplayEffectAttributeCaptureDefinition BaseHealStaticsDef;
	
	
	FFPSCombatHealStatics()
	{
		BaseHealStaticsDef = FGameplayEffectAttributeCaptureDefinition(UFPSCombatAttributeSet::GetHealAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
	}
};

static const FFPSCombatHealStatics& HealStatics()
{
	static const FFPSCombatHealStatics HStatics;
	return HStatics;
}



UFPSCombatHealExecCalc::UFPSCombatHealExecCalc()
{
	RelevantAttributesToCapture.Add(HealStatics().BaseHealStaticsDef);
}

void UFPSCombatHealExecCalc::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
#if WITH_SERVER_CODE
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	
	FAggregatorEvaluateParameters EvaluateParams;

	EvaluateParams.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvaluateParams.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	

	float BaseHeal = 0.f;
	
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude( HealStatics().BaseHealStaticsDef, EvaluateParams, BaseHeal);

	const float HealDone = FMath::Max(BaseHeal, 0.f);

	if (HealDone > 0.f)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UFPSCombatAttributeSet::GetHealAttribute(), EGameplayModOp::Additive, HealDone));
	}
#endif // #if WITH_SERVER_CODE
}
