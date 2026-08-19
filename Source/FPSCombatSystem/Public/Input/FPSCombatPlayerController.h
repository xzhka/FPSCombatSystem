// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"
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

	AFPSCombatPlayerController(const FObjectInitializer& ObjectInitializer);
	
protected:
	virtual void BeginPlay() override;

public:
	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> HUDWidgetClass;

	UPROPERTY()
	TObjectPtr<UUserWidget> HUDWidgetInstance;
	
};
