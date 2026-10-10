// FPS Combat project


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
	
	// Only the local player has a HUD.
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
			// Ability input is processed after the engine has gathered this
			// frame's input.
			ASC->ProcessAbilityInput(DeltaTime, bGamePaused);
		}
	}
	
	Super::PostProcessInput(DeltaTime, bGamePaused);
}