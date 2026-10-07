// FPS Combat project

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystem/FPSCombatAbilitySet.h"
#include "AbilitySystem/FPSCombatAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/FPSCombatAttributeSet.h"
#include "Items/FPSCombatItemManagerComponent.h"
#include "FPSCombatPlayerState.generated.h"


/** AFPSCombatPlayerState
 *
 *  Base player state class for pawns
 */
UCLASS()
class FPSCOMBATSYSTEM_API AFPSCombatPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

	AFPSCombatPlayerState();
	
public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UAttributeSet* GetAttributeSet() const;

	void GrantDefaultAbilities();
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TObjectPtr<UFPSCombatAbilitySystemComponent> ASC;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attribute")
	TObjectPtr<UFPSCombatAttributeSet> AttributeSet;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UFPSCombatItemManagerComponent> ItemComponent;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UFPSCombatAbilitySet> AbilitySet;
	
	FCombatAbilitySet_GrantedHandles GrantedHandles;
	
};
