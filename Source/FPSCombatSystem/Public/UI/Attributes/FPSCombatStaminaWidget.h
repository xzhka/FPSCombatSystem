// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Attributes/FPSCombatHUDElementWidget.h"
#include "FPSCombatStaminaWidget.generated.h"

/**
 * 
 */
UCLASS()
class FPSCOMBATSYSTEM_API UFPSCombatStaminaWidget : public UFPSCombatHUDElementWidget
{
	GENERATED_BODY()


protected:

	UPROPERTY(BlueprintReadOnly, Category = "Stamina")
	TWeakObjectPtr<UFPSCombatBaseComponent> StaminaComponent;

	UFUNCTION(BlueprintImplementableEvent)
	void OnStaminaUpdated(float Percent);
	
	UFUNCTION()
	void HandleStaminaPercentUpdated(const float Percent) { OnStaminaUpdated(Percent); }
	
	virtual void NativeDestruct() override;
	virtual void HandlePawnChanged(APawn* OldPawn, APawn* NewPawn) override;
	
};
