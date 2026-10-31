// FPS Combat project

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "NiagaraFunctionLibrary.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "FPSCombatImpactDefinition.generated.h"


/* Info set for impact effects */
USTRUCT(BlueprintType)
struct FImpactEffectSet
{
GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Impact")
	TObjectPtr<UMaterialInterface> ImpactMaterial = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Impact")
	TObjectPtr<UNiagaraSystem> ImpactEffect = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Impact")
	TObjectPtr<USoundBase> ImpactSound = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Impact")
	FVector DecalSize = FVector(0.f, 0.f, 0.f);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Impact")
	float DecalLifeSpan = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Impact")
	float DecalFadeOutTime = 0.f;
};


/*	UFPSCombatImpactDefinition
 *	
 *	Asset for impact effects
 */
UCLASS(Blueprintable, BlueprintType)
class FPSCOMBATSYSTEM_API UFPSCombatImpactDefinition : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Impact")
	TMap<TEnumAsByte<EPhysicalSurface>, FImpactEffectSet> SurfaceEffects;

	UPROPERTY(EditDefaultsOnly, Category = "Impact")
	FImpactEffectSet DefaultEffectSet;

	UFUNCTION(BlueprintPure, Category = "Impact")
	const FImpactEffectSet& GetEffectSetFromSurface(EPhysicalSurface Surface) const
	{
		if (const FImpactEffectSet* EffectSet = SurfaceEffects.Find(Surface))
		{
			return *EffectSet;
		}
		return DefaultEffectSet;
	}
};
