// FPS Combat project


#include "GameModes/FPSCombatGameMode.h"

#include "AI/FPSCombatEnemyController.h"
#include "Characters/FPSCombatCharacter.h"
#include "Characters/FPSCombatPawnData.h"
#include "GameModes/FPSCombatGameState.h"
#include "GameModes/FPSCombatPlayerState.h"
#include "Input/FPSCombatPlayerController.h"
#include "Spawning/FPSCombatSpawningManagerComponent.h"

AFPSCombatGameMode::AFPSCombatGameMode()
{
	DefaultPawnClass = AFPSCombatCharacter::StaticClass();
	PlayerControllerClass = AFPSCombatPlayerController::StaticClass();
	PlayerStateClass = AFPSCombatPlayerState::StaticClass();
	GameStateClass = AFPSCombatGameState::StaticClass();
}

const UFPSCombatPawnData* AFPSCombatGameMode::GetPlayerPawnData(const AController* InController) const
{
	if (InController != nullptr)
	{
		if (AFPSCombatPlayerState* PlayerState = InController->GetPlayerState<AFPSCombatPlayerState>())
		{
			if (const UFPSCombatPawnData* PawnData = PlayerState->GetPawnData<UFPSCombatPawnData>())
			{
				return PawnData;
			}
		}

		if (DefaultEnemyData && InController->IsA<AFPSCombatEnemyController>())
		{
			return DefaultEnemyData;
		}
	}
	return DefaultPawnData;
}

void AFPSCombatGameMode::HandlePawnDeath(AController* Controller)
{
	if (!IsValid(Controller)) return;

	// One respawn timer per controller.
	if (RespawnTimers.Contains(Controller)) return;

	FTimerHandle& Handle = RespawnTimers.Add(Controller);

	FTimerDelegate Delegate = FTimerDelegate::CreateUObject(this, &AFPSCombatGameMode::RespawnPlayer, Controller);
	GetWorldTimerManager().SetTimer(Handle, Delegate, RespawnDelay, false);
}

AActor* AFPSCombatGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	if (const UFPSCombatSpawningManagerComponent* SpawningComponent = GameState->FindComponentByClass<UFPSCombatSpawningManagerComponent>())
	{
		return SpawningComponent->ChoosePlayerStart(Player);
	}
	
	return Super::ChoosePlayerStart_Implementation(Player);
}

UClass* AFPSCombatGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	if (const UFPSCombatPawnData* PawnData = GetPlayerPawnData(InController))
	{
		return PawnData->PawnClass;	
	}
	
	return Super::GetDefaultPawnClassForController_Implementation(InController);
}

void AFPSCombatGameMode::RespawnPlayer(AController* Controller)
{
	RespawnTimers.Remove(Controller);

	if (!IsValid(Controller)) return;

	// Detach any leftover pawn before restarting the player.
	if (Controller->GetPawn())
	{
		Controller->UnPossess();
	}
	RestartPlayer(Controller);
}

void AFPSCombatGameMode::Logout(AController* Exiting)
{
	// Cancel a pending respawn if the player leaves.
	if (FTimerHandle* Handle = RespawnTimers.Find(Exiting))
	{
		GetWorldTimerManager().ClearTimer(*Handle);
		RespawnTimers.Remove(Exiting);
	}
	
	Super::Logout(Exiting);
}
