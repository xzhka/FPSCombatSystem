// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Exec/FPSCombatDamageExecCalc.h"

#include "AbilitySystem/Attributes/FPSCombatAttributeSet.h"

struct FFPSCombatDamageStatics
{
	FGameplayEffectAttributeCaptureDefinition BaseDamageStaticsDef;

	FFPSCombatDamageStatics()
	{
		BaseDamageStaticsDef = FGameplayEffectAttributeCaptureDefinition(UFPSCombatAttributeSet::GetDamageAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
	}
	
};

static const FFPSCombatDamageStatics& DamageStatics()
{
	static FFPSCombatDamageStatics DStatics;
	return DStatics;
}


UFPSCombatDamageExecCalc::UFPSCombatDamageExecCalc()
{
	RelevantAttributesToCapture.Add(DamageStatics().BaseDamageStaticsDef);
}

void UFPSCombatDamageExecCalc::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
#if WITH_SERVER_CODE
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	
	FAggregatorEvaluateParameters EvaluateParams;
	EvaluateParams.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvaluateParams.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	float BaseDamage = 0.f;
	
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BaseDamageStaticsDef, EvaluateParams, BaseDamage);

	BaseDamage += FMath::Max(Spec.GetSetByCallerMagnitude(FName("SetByCaller.Data.Damage"), false, 0.f), 0.f);
	
	const float DamageDone = FMath::Max(BaseDamage, 0.f);

	if (DamageDone > 0.f)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UFPSCombatAttributeSet::GetDamageAttribute(), EGameplayModOp::Additive, DamageDone));
	}
#endif // #if WITH_SERVER_CODE
}
