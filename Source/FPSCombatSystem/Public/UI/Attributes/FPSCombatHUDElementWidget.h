// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Characters/Components/FPSCombatBaseComponent.h"
#include "FPSCombatHUDElementWidget.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class FPSCOMBATSYSTEM_API UFPSCombatHUDElementWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	virtual void HandlePawnChanged(APawn* OldPawn, APawn* NewPawn) { }
	
	
	void RebindComp(TWeakObjectPtr<UFPSCombatBaseComponent>& BaseComp, UFPSCombatBaseComponent* Component, FOnPercentChanged::FDelegate Delegate);

private:
	UFUNCTION()
	void OnPossessedPawnChangedInternal(APawn* OldPawn, APawn* NewPawn) { HandlePawnChanged(OldPawn, NewPawn); }
	
};
