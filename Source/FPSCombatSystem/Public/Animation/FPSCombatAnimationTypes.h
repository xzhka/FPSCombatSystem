#pragma once
#include "GameplayTagContainer.h"

#include "FPSCombatAnimationTypes.generated.h"

USTRUCT(BlueprintType)
struct FFPSCombatLayerTypeSet
{
	GENERATED_BODY()

public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	TSubclassOf<UAnimInstance> AnimLayer;
};

USTRUCT(BlueprintType)
struct FFPSCombatLayerSelectionSet
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	TArray<FFPSCombatLayerTypeSet> CompleteAnimLayers;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	TSubclassOf<UAnimInstance> DefaultLayer;

	TSubclassOf<UAnimInstance> SelectLayers() const;
	
};
