// FPS Combat project

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnComponent.h"
#include "Input/FPSCombatInputConfig.h"
#include "FPSCombatCharacterPawnComp.generated.h"

struct FInputActionValue;
class UInputAction;


/** UFPSCombatCharacterPawnComp
 *
 *  Pawn component which stores the input
 */
UCLASS()
class FPSCOMBATSYSTEM_API UFPSCombatCharacterPawnComp : public UPawnComponent
{
	GENERATED_BODY()
	/* Input actions */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

public:

	/* Base component initializing */
	void InitializeInputComponents(UInputComponent* PlayerInputComponent);
	

protected:
	/* Default pawn class actions */
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Jump();
	void StopJump();

	/* Ability input action treatment */
	void Ability_InputTagPressed(FGameplayTag InputTag);
	void Ability_InputTagReleased(FGameplayTag InputTag);
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UFPSCombatInputConfig> InputConfig;
	
};
