// FPS Combat project

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "FPSCombatTagsRelationshipMapping.generated.h"

/* Struct which describe the connection between different tags*/
USTRUCT()
struct FFPSCombatTagRelationship
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Ability)
	FGameplayTag AbilityTag;
	
	UPROPERTY(EditAnywhere, Category = Ability)
	FGameplayTagContainer TagsToBlock;

	UPROPERTY(EditAnywhere, Category = Ability)
	FGameplayTagContainer TagsToCancel;

	UPROPERTY(EditAnywhere, Category = Ability)
	FGameplayTagContainer ActivationRequiredTags;

	UPROPERTY(EditAnywhere, Category = Ability)
	FGameplayTagContainer ActivationBlockedTags;
};



/*	UFPSCombatTagsRelationshipMapping
 *	
 *	Data asset which holds tags relations 
 */
UCLASS()
class FPSCOMBATSYSTEM_API UFPSCombatTagsRelationshipMapping : public UDataAsset
{
	GENERATED_BODY()

public:
	void GetAbilityTagsToBlockAndCancel(const FGameplayTagContainer& AbilityTags, FGameplayTagContainer* OutTagsToBlock, FGameplayTagContainer* OutTagsToCancel) const;
	void GetRequiredAndBlockedTags(const FGameplayTagContainer& AbilityTags, FGameplayTagContainer* OutActiveRequired, FGameplayTagContainer* OutActiveBlocked);
	bool IsAbilityCanceledByTag(const FGameplayTagContainer& AbilityTags, const FGameplayTag& InputTag) const;

private:
	UPROPERTY(EditAnywhere, Category = Ability)
	TArray<FFPSCombatTagRelationship> AbilityTagsRelationships;
	
};
