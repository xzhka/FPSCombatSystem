// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FPSCombatInfoWidget.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class FPSCOMBATSYSTEM_API UFPSCombatInfoWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void SetVisibility(ESlateVisibility InVisibility) override;	

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	UPROPERTY(EditAnywhere, Category = "HUD")
	ESlateVisibility ShownVisibility = ESlateVisibility::Visible;
	
	UPROPERTY(EditAnywhere, Category = "HUD")
	ESlateVisibility HiddenVisibility = ESlateVisibility::Collapsed;
private:

	bool bWantsToVisible = true;
	
};
