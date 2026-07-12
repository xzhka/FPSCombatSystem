// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Characters/Components/FPSCombatHealthComponent.h"
#include "Characters/Components/FPSCombatStaminaComponent.h"
#include "FPSCombatHUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class FPSCOMBATSYSTEM_API UFPSCombatHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	
	void RebindComp(TWeakObjectPtr<UFPSCombatBaseComponent>& BaseComp, UFPSCombatBaseComponent* Component, FOnPercentChanged::FDelegate Delegate);
	
protected:
	UPROPERTY(BlueprintReadOnly, Category = "Health")
	TWeakObjectPtr<UFPSCombatBaseComponent> HealthComponent;

	UPROPERTY(BlueprintReadOnly, Category = "Stamina")
	TWeakObjectPtr<UFPSCombatBaseComponent> StaminaComponent;
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnHealthUpdated(float Percent);

	UFUNCTION(BlueprintImplementableEvent)
	void OnStaminaUpdated(float Percent);
	
	UFUNCTION()
	void HandleHealthPercentUpdated(const float Percent) { OnHealthUpdated(Percent); };

	UFUNCTION()
	void HandleStaminaPercentUpdated(const float Percent) { OnStaminaUpdated(Percent); };
	
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION()
	void HandlePawnChanged(APawn* OldPawn, APawn* NewPawn);
	
};
