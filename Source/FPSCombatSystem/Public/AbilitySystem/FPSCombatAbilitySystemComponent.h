// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "FPSCombatTagsRelationshipMapping.h"
#include "FPSCombatAbilitySystemComponent.generated.h"

/**
 * 
 */
UCLASS()
class FPSCOMBATSYSTEM_API UFPSCombatAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:

	FORCEINLINE UFPSCombatTagsRelationshipMapping* GetRelationshipMapping() const { return RelationshipMapping; }


	
	UFPSCombatAbilitySystemComponent();
	
	/*Adding abilities by InputTag inside SpecHandles*/
	void AbilityInputTagPressed(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);

	/*Initialize Default Gameplay Effects*/
	void InitializeDefaultAttributes();
	
	void ProcessAbilityInput(float DeltaTime, bool bGamePaused);
	
	void ClearAbilityInput();
	virtual void ApplyAbilityBlockAndCancelTags(const FGameplayTagContainer& AbilityTags,
		UGameplayAbility* RequestingAbility, bool bEnableBlockTags, const FGameplayTagContainer& BlockTags,
		bool bExecuteCancelTags, const FGameplayTagContainer& CancelTags) override;
	
protected:
	/*Array`s of SpecHandles*/
	TArray<FGameplayAbilitySpecHandle> PressedAbilitySpecHandles;
	TArray<FGameplayAbilitySpecHandle> ReleasedAbilitySpecHandles;
	TArray<FGameplayAbilitySpecHandle> HeldAbilitySpecHandles;

	UPROPERTY(EditDefaultsOnly, Category = "DefaultEffects")
	TArray<TSubclassOf<UGameplayEffect>> DefaultGameplayEffect;

	UPROPERTY(EditDefaultsOnly, Category = "TagMapping")
	TObjectPtr<UFPSCombatTagsRelationshipMapping> RelationshipMapping;
	
	
};
