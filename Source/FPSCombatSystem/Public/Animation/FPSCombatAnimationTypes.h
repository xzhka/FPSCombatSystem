// FPS Combat project

#pragma once
#include "GameplayTagContainer.h"

#include "FPSCombatAnimationTypes.generated.h"

/* Struct handling anim instance layer*/
USTRUCT(BlueprintType)
struct FFPSCombatLayerTypeSet
{
	GENERATED_BODY()

public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	TSubclassOf<UAnimInstance> AnimLayer;
};


/*	FFPSCombatLayerSelectionSet
 *	
 *	Selection set with complete
 *	animation layers
 */
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
