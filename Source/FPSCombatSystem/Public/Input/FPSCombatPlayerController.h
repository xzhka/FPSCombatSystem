// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FPSCombatInputConfig.h"
#include "UI/FPSCombatHeathBarWidget.h"
#include "FPSCombatPlayerController.generated.h"

struct FInputActionValue;
class UInputMappingContext;
class UInputAction;

/**
 * 
 */
UCLASS()
class FPSCOMBATSYSTEM_API AFPSCombatPlayerController : public APlayerController
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;


public:
	virtual void AcknowledgePossession(APawn* P) override;
	
protected:
	virtual void SetupInputComponent() override;
	virtual void BeginPlay() override;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

	void Jump();
	void StopJump();
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UFPSCombatInputConfig> InputConfig;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UFPSCombatHeathBarWidget> HealthWidgetClass;

	UPROPERTY()
	TObjectPtr<UFPSCombatHeathBarWidget> HealthWidget;
	
};
