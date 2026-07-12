// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayTagBase.h"
#include "AbilityTask_WaitCancelTags.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCancelTagTriggered);


/**
 * 
 */
UCLASS()
class FPSCOMBATSYSTEM_API UAbilityTask_WaitCancelTags : public UAbilityTask_WaitGameplayTag
{
	GENERATED_BODY()

public:
	virtual void Activate() override;
	
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static UAbilityTask_WaitCancelTags* WaitCancelTags(UGameplayAbility* OwningAbility, FGameplayTagContainer TagsToAdd, FGameplayTagContainer TagsToRemove);
	
protected:
	virtual void OnDestroy(bool AbilityIsEnding) override;

public:
	UPROPERTY(BlueprintAssignable)
	FOnCancelTagTriggered OnCancelTagTriggered;


private:
	FGameplayTagContainer AddedTags;
	FGameplayTagContainer RemovedTags;

	TArray<TPair<FGameplayTag, FDelegateHandle>> RegisteredHandles;
};
