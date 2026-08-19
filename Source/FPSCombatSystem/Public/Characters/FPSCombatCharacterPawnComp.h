// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnComponent.h"
#include "Input/FPSCombatInputConfig.h"
#include "FPSCombatCharacterPawnComp.generated.h"

struct FInputActionValue;
class UInputAction;
/**
 * 
 */
UCLASS()
class FPSCOMBATSYSTEM_API UFPSCombatCharacterPawnComp : public UPawnComponent
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

public:
	
	void InitializeInputComponents(UInputComponent* PlayerInputComponent);
	

protected:

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

	void Ability_InputTagPressed(FGameplayTag InputTag);
	void Ability_InputTagReleased(FGameplayTag InputTag);

	void Jump();
	void StopJump();
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UFPSCombatInputConfig> InputConfig;
	
};
