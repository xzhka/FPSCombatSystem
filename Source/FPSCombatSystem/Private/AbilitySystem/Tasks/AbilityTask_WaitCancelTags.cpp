// FPS Combat project


#include "AbilitySystem/Tasks/AbilityTask_WaitCancelTags.h"

#include "AbilitySystemComponent.h"

void UAbilityTask_WaitCancelTags::Activate()
{
	Super::Activate();
	
	if (!AbilitySystemComponent.IsValid()) return;

	for (const FGameplayTag& CountTag : AddedTags)
	{
		FDelegateHandle Handle = AbilitySystemComponent->RegisterGameplayTagEvent(CountTag, EGameplayTagEventType::NewOrRemoved)
			.AddLambda([this](const FGameplayTag, int32 NewCount)
		{
			if (NewCount > 0)
			{
				OnCancelTagTriggered.Broadcast();
			}
		});
		RegisteredHandles.Add({CountTag, Handle});
		if (AbilitySystemComponent->GetTagCount(CountTag) > 0 ) { OnCancelTagTriggered.Broadcast(); }
	}

	for (const FGameplayTag& CountTag : RemovedTags)
	{
		FDelegateHandle Handle = AbilitySystemComponent->RegisterGameplayTagEvent(CountTag, EGameplayTagEventType::NewOrRemoved)
			.AddLambda([this](const FGameplayTag, int32 NewCount)
		{
			if (NewCount == 0)
			{
				OnCancelTagTriggered.Broadcast();
			}
		});
		RegisteredHandles.Add({CountTag, Handle});
		if (AbilitySystemComponent->GetTagCount(CountTag) == 0 ) { OnCancelTagTriggered.Broadcast(); }
	}
}

UAbilityTask_WaitCancelTags* UAbilityTask_WaitCancelTags::WaitCancelTags(UGameplayAbility* OwningAbility,
	FGameplayTagContainer TagsToAdd, FGameplayTagContainer TagsToRemove)
{
	UAbilityTask_WaitCancelTags* Task = NewAbilityTask<UAbilityTask_WaitCancelTags>(OwningAbility);
	Task->AddedTags = TagsToAdd;
	Task->RemovedTags = TagsToRemove;

	return Task;
}

void UAbilityTask_WaitCancelTags::OnDestroy(bool AbilityIsEnding)
{

	if (AbilitySystemComponent.IsValid())
	{
		for (const auto& Pair : RegisteredHandles)
		{
			AbilitySystemComponent->RegisterGameplayTagEvent(Pair.Key, EGameplayTagEventType::NewOrRemoved).Remove(Pair.Value);
		}
	}
	
	Super::OnDestroy(AbilityIsEnding);
	
}
