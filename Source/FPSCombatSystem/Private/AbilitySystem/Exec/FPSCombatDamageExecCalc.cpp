// FPS Combat project


#include "AbilitySystem/Exec/FPSCombatDamageExecCalc.h"

#include "AbilitySystem/Attributes/FPSCombatAttributeSet.h"
#include "FPSCombatSystem/FPSCombatGameplayTags.h"
#include "Physics/PhysicMaterialWithTag.h"
#include "Weapons/FPSCombatRangedWeaponInstance.h"

namespace 
{
	float GetZoneMultiplier(FGameplayEffectContextHandle Context)
	{
		if (const FHitResult* HitResult = Context.GetHitResult())
		{
			if (const UPhysicMaterialWithTag* PM = Cast<UPhysicMaterialWithTag>(HitResult->PhysMaterial.Get()))
			{
				if (const UFPSCombatRangedWeaponInstance* WeaponInstance = Cast<const UFPSCombatRangedWeaponInstance>(Context.GetSourceObject()))
				{
					float CombinedMultiplier = 1.f;
					for (const FGameplayTag MaterialTag : PM->Tags)
					{
						if (const float* TagMultiplier = WeaponInstance->GetWeaponDefinition()->ZoneMultipliers.Find(MaterialTag))
						{
							CombinedMultiplier *= *TagMultiplier;
						}
					}
					return CombinedMultiplier;
				}
			}
		}
		return 1.f;
	}
}

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
	FGameplayEffectContextHandle ContextHandle = Spec.GetContext();
		
	FAggregatorEvaluateParameters EvaluateParams;
	EvaluateParams.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvaluateParams.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	float BaseDamage = 0.f;
	
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BaseDamageStaticsDef, EvaluateParams, BaseDamage);

	// Get damage magnitude from gameplay tag
	BaseDamage += FMath::Max(Spec.GetSetByCallerMagnitude(FPSCombatGameplayTags::SetByCaller_Data_Damage, false, 0.f), 0.f);

	BaseDamage *= GetZoneMultiplier(ContextHandle);
	
	const float DamageDone = FMath::Max(BaseDamage, 0.f);

	if (DamageDone > 0.f)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UFPSCombatAttributeSet::GetDamageAttribute(), EGameplayModOp::Additive, DamageDone));
	}
#endif // #if WITH_SERVER_CODE
}
