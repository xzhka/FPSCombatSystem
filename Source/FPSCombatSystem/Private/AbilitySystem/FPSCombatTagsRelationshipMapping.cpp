// FPS Combat project


#include "AbilitySystem/FPSCombatTagsRelationshipMapping.h"

void UFPSCombatTagsRelationshipMapping::GetAbilityTagsToBlockAndCancel(const FGameplayTagContainer& AbilityTags,
	FGameplayTagContainer* OutTagsToBlock, FGameplayTagContainer* OutTagsToCancel) const
{
	for (int32 i=0; i < AbilityTagsRelationships.Num(); i++)
	{
		const FFPSCombatTagRelationship& Tag = AbilityTagsRelationships[i];
		if (AbilityTags.HasTag(Tag.AbilityTag))
		{
			if (OutTagsToBlock)
			{
				OutTagsToBlock->AppendTags(Tag.TagsToBlock);
			}
			if (OutTagsToCancel)
			{
				OutTagsToCancel->AppendTags(Tag.TagsToCancel);
			}
		}
	}
}

void UFPSCombatTagsRelationshipMapping::GetRequiredAndBlockedTags(const FGameplayTagContainer& AbilityTags,
	FGameplayTagContainer* OutActiveRequired, FGameplayTagContainer* OutActiveBlocked)
{
	for (int32 i=0; i < AbilityTagsRelationships.Num(); i++)
	{
		const FFPSCombatTagRelationship& Tag = AbilityTagsRelationships[i];
		if (AbilityTags.HasTag(Tag.AbilityTag))
		{
			if (OutActiveRequired)
			{
				OutActiveRequired->AppendTags(Tag.ActivationRequiredTags);
			}
			if (OutActiveBlocked)
			{
				OutActiveBlocked->AppendTags(Tag.ActivationBlockedTags);
			}
		}
	}
}

bool UFPSCombatTagsRelationshipMapping::IsAbilityCanceledByTag(const FGameplayTagContainer& AbilityTags,
	const FGameplayTag& InputTag) const
{	
	for (int32 i=0; i < AbilityTagsRelationships.Num(); i++)
	{
		const FFPSCombatTagRelationship& Tag = AbilityTagsRelationships[i];
		if (Tag.AbilityTag == InputTag && Tag.TagsToCancel.HasAny(AbilityTags))
		{
			return true;
		}
	}
	return false;
}
