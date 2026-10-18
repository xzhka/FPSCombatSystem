// FPS Combat project

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystem/FPSCombatAbilitySet.h"
#include "AbilitySystem/FPSCombatAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/FPSCombatAttributeSet.h"
#include "Characters/FPSCombatPawnData.h"
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

	template<typename T>
	const T* GetPawnData() const { return Cast<T>(PawnData); }

	void SetPawnData(const UFPSCombatPawnData* InPawnData);
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UAttributeSet* GetAttributeSet() const;

	virtual void PostInitializeComponents() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TObjectPtr<UFPSCombatAbilitySystemComponent> ASC;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attribute")
	TObjectPtr<UFPSCombatAttributeSet> AttributeSet;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UFPSCombatItemManagerComponent> ItemComponent;

	UFUNCTION()
	void OnRep_PawnData();
	
	
	UPROPERTY(ReplicatedUsing = OnRep_PawnData)
	TObjectPtr<const UFPSCombatPawnData> PawnData;
	
};
