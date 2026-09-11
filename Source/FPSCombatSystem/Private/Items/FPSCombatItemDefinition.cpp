// Fill out your copyright notice in the Description page of Project Settings.


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
