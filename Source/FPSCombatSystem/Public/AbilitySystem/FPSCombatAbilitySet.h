// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FPSCombatAbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"
#include "Engine/DataAsset.h"
#include "FPSCombatAbilitySet.generated.h"

USTRUCT(BlueprintType)
struct FCombatAbilitySet_Abilities
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayAbility> GameplayAbility;

	UPROPERTY(EditDefaultsOnly)
	int32 AbilityLevel = 1;

	UPROPERTY(EditDefaultsOnly, meta = (Category = "InputTag"))
	FGameplayTag InputTag;
};

USTRUCT(BlueprintType)
struct FCombatAbilitySet_Effects
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> GameplayEffect;

	UPROPERTY(EditDefaultsOnly)
	int32 EffectLevel = 1;
};

USTRUCT(BlueprintType)
struct FCombatAbilitySet_AttributeSets
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAttributeSet> AttributeSet;
};

USTRUCT(BlueprintType)
struct FCombatAbilitySet_GrantedHandles
{
	GENERATED_BODY()

public:
	
	void AddAbilities(const FGameplayAbilitySpecHandle& Handle);
	void AddGameplayEffects(const FActiveGameplayEffectHandle& Handle);
	void AddAttributeSets(UAttributeSet* Set);

	void ClearAbilitySystem(UFPSCombatAbilitySystemComponent* ASC);
	

protected:

	UPROPERTY()
	TArray<FGameplayAbilitySpecHandle> AbilitySpecHandles;

	UPROPERTY()
	TArray<FActiveGameplayEffectHandle> EffectSpecHandles;

	UPROPERTY()
	TArray<TObjectPtr<UAttributeSet>> AttributeSets;
	
};



/**
 * 
 */
UCLASS()
class FPSCOMBATSYSTEM_API UFPSCombatAbilitySet : public UDataAsset
{
	GENERATED_BODY()
	
public:
	
	void GiveAbility(UFPSCombatAbilitySystemComponent* ASC, FCombatAbilitySet_GrantedHandles* GrantedHandles,UObject* SpecObject = nullptr) const;
	
protected:
		
	UPROPERTY(EditDefaultsOnly, Category = "Abilities")
	TArray<FCombatAbilitySet_Abilities> GrantedGameplayAbilities;

	UPROPERTY(EditDefaultsOnly, Category = "Abilities")
	TArray<FCombatAbilitySet_Effects> GrantedGameplayEffects;

	UPROPERTY(EditDefaultsOnly, Category = "Abilities")
	TArray<FCombatAbilitySet_AttributeSets> GrantedGameplayAttributes;

	
};
