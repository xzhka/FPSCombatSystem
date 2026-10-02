
#include <FPSCombatSystem/Public/Animation/FPSCombatAnimationTypes.h>


TSubclassOf<UAnimInstance> FFPSCombatLayerSelectionSet::SelectLayers() const
{
	for (const FFPSCombatLayerTypeSet& Rule : CompleteAnimLayers)
	{
		if (Rule.AnimLayer!= nullptr)
		{
			return Rule.AnimLayer;
		}
	}

	
	return DefaultLayer;
}
