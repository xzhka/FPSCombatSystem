// Fill out your copyright notice in the Description page of Project Settings.


#include "Input/FPSCombatPlayerController.h"

#include "Camera/FPSCombatPlayerCameraManager.h"
#include "GameModes/FPSCombatPlayerState.h"

AFPSCombatPlayerController::AFPSCombatPlayerController(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	PlayerCameraManagerClass = AFPSCombatPlayerCameraManager::StaticClass();

	QuickBarComponent = CreateDefaultSubobject<UFPSCombatQuickBarComponent>(TEXT("QuickBarComponent"));
}

void AFPSCombatPlayerController::BeginPlay()
{
	Super::BeginPlay();
	

	if (HUDWidgetClass && IsLocalController())
	{
		HUDWidgetInstance = CreateWidget<UUserWidget>(this, HUDWidgetClass);
		if (HUDWidgetInstance)
		{
			HUDWidgetInstance->AddToViewport();
		}
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