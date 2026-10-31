// FPS Combat project


#include "Items/FPSCombatItemDefinition.h"

void UFPSCombatItemFragment_Stats::OnInstanceCreated(UFPSCombatItemInstance* InItemInstance) const
{
	for (const auto& Fragments : Stats)
	{
		InItemInstance->AddStackCount(Fragments.Key, Fragments.Value);
	}
}

int32 UFPSCombatItemFragment_Stats::GetStatsByTag(FGameplayTag InTag) const
{
	if (const int32* StatValuePtr = Stats.Find(InTag))
	{
		return *StatValuePtr;
	}
	return 0;
}

const UFPSCombatItemFragment* UFPSCombatItemDefinition::FindFragmentByClass(
	TSubclassOf<UFPSCombatItemFragment> FragmentClass) const
{
	if (FragmentClass != nullptr)
	{
		for (UFPSCombatItemFragment* Fragment : Fragments)
		{
			if (Fragment && Fragment->IsA(FragmentClass))
			{
				return Fragment;
			}
		}
	}
	return nullptr;
}

int32 UFPSCombatItemDefinition::GetDefaultStatsValueByTag(FGameplayTag InTag) const
{
	if (const UFPSCombatItemFragment_Stats* Fragment_Stats = Cast<UFPSCombatItemFragment_Stats>(FindFragmentByClass(UFPSCombatItemFragment_Stats::StaticClass())))
	{
		return Fragment_Stats->GetStatsByTag(InTag);
	}
	return 0;
}
