// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "FPSCombatAbilitySystemComponent.generated.h"

/**
 * 
 */
UCLASS()
class FPSCOMBATSYSTEM_API UFPSCombatAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	UFPSCombatAbilitySystemComponent();
	
	void AbilityInputTagPressed(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);
	
	void InitializeDefaultAttributes();
	void ProcessAbilityInput(float DeltaTime, bool bGamePaused);
	
	void ClearAbilityInput();

protected:

	TArray<FGameplayAbilitySpecHandle> PressedAbilitySpecHandles;
	TArray<FGameplayAbilitySpecHandle> ReleasedAbilitySpecHandles;
	TArray<FGameplayAbilitySpecHandle> HeldAbilitySpecHandles;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> DefaultGameplayEffect;
};
