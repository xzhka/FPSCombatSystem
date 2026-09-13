
#include <FPSCombatSystem/Public/Animation/FPSCombatAnimationTypes.h>


TSubclassOf<UAnimInstance> FFPSCombatLayerSelectionSet::SelectLayers(const FGameplayTagContainer& CosmeticTags) const
{
	for (const FFPSCombatLayerTypeSet& Rule : CompleteAnimLayers)
	{
		if ((Rule.AnimLayer!= nullptr) && CosmeticTags.HasAll(Rule.RequiredTag))
		{
			return Rule.AnimLayer;
		}
	}

	
	return DefaultLayer;
}
