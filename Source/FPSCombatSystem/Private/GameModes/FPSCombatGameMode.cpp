// FPS Combat project


#include "GameModes/FPSCombatGameMode.h"

#include "Characters/FPSCombatCharacter.h"
#include "GameModes/FPSCombatPlayerState.h"
#include "Input/FPSCombatPlayerController.h"

AFPSCombatGameMode::AFPSCombatGameMode()
{
	DefaultPawnClass = AFPSCombatCharacter::StaticClass();
	PlayerControllerClass = AFPSCombatPlayerController::StaticClass();
	PlayerStateClass = AFPSCombatPlayerState::StaticClass();
}

void AFPSCombatGameMode::HandlePawnDeath(AController* Controller)
{
	if (!IsValid(Controller)) return;
	if (RespawnTimers.Contains(Controller)) return;

	FTimerHandle& Handle = RespawnTimers.Add(Controller);

	FTimerDelegate Delegate = FTimerDelegate::CreateUObject(this, &AFPSCombatGameMode::RespawnPlayer, Controller);
	GetWorldTimerManager().SetTimer(Handle, Delegate, RespawnDelay, false);
}

void AFPSCombatGameMode::RespawnPlayer(AController* Controller)
{
	RespawnTimers.Remove(Controller);

	if (!IsValid(Controller)) return;

	if (Controller->GetPawn())
	{
		Controller->UnPossess();
	}
	RestartPlayer(Controller);
}

void AFPSCombatGameMode::Logout(AController* Exiting)
{
	if (FTimerHandle* Handle = RespawnTimers.Find(Exiting))
	{
		GetWorldTimerManager().ClearTimer(*Handle);
		RespawnTimers.Remove(Exiting);
	}
	
	Super::Logout(Exiting);
}
