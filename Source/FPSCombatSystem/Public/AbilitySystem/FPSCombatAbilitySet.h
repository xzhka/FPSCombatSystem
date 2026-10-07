// FPS Combat project

#pragma once

#include "CoreMinimal.h"
#include "FPSCombatAbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"
#include "Engine/DataAsset.h"
#include "FPSCombatAbilitySet.generated.h"

/** FCombatAbilitySet_Abilities
 *
 * Information about gameplay abilities adding parameters
 */
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

/** FCombatAbilitySet_Effects
 *
 * Information about gameplay effects adding parameters
 */
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


/** FCombatAbilitySet_GrantedHandles
 *
 * A piece of granted handles with effect, attributes and abilities 
 */
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



/** UFPSCombatAbilitySet
 *
 * Data asset used to grant attribute sets/abilities/effects
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
