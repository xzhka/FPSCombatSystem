// Fill out your copyright notice in the Description page of Project Settings.


#include "Input/FPSCombatPlayerController.h"

#include "GameModes/FPSCombatPlayerState.h"


void AFPSCombatPlayerController::BeginPlay()
{
	Super::BeginPlay();
	

	if (HealthWidgetClass && IsLocalController())
	{
		HealthWidget = CreateWidget<UFPSCombatHUDWidget>(this, HealthWidgetClass);
		HealthWidget->AddToViewport();
	}
}

void AFPSCombatPlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	if (AFPSCombatPlayerState* PS = GetPlayerState<AFPSCombatPlayerState>())
	{
		if (UFPSCombatAbilitySystemComponent* ASC = Cast<UFPSCombatAbilitySystemComponent>(PS->GetAbilitySystemComponent()))
		{
			ASC->ProcessAbilityInput(DeltaTime, bGamePaused);
		}
	}
	
	Super::PostProcessInput(DeltaTime, bGamePaused);
}