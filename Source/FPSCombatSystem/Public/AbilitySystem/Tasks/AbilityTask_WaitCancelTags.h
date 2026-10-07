// FPS Combat project

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayTagBase.h"
#include "AbilityTask_WaitCancelTags.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCancelTagTriggered);


/** UAbilityTask_WaitCancelTags
 *
 * Ability task related to
 * new added tags while ability is active
 */
UCLASS()
class FPSCOMBATSYSTEM_API UAbilityTask_WaitCancelTags : public UAbilityTask_WaitGameplayTag
{
	GENERATED_BODY()

public:
	virtual void Activate() override;
	
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static UAbilityTask_WaitCancelTags* WaitCancelTags(UGameplayAbility* OwningAbility, FGameplayTagContainer TagsToAdd, FGameplayTagContainer TagsToRemove);

	UPROPERTY(BlueprintAssignable)
	FOnCancelTagTriggered OnCancelTagTriggered;
protected:
	virtual void OnDestroy(bool AbilityIsEnding) override;

private:
	FGameplayTagContainer AddedTags;
	FGameplayTagContainer RemovedTags;

	TArray<TPair<FGameplayTag, FDelegateHandle>> RegisteredHandles;
};
