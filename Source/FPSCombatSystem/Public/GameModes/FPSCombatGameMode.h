// FPS Combat project

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FPSCombatGameMode.generated.h"

/** AFPSCombatGameMode
 *
 *  Actor manager class which sets up game
 */
UCLASS()
class FPSCOMBATSYSTEM_API AFPSCombatGameMode : public AGameModeBase
{
	GENERATED_BODY()

	AFPSCombatGameMode();

public:
	void HandlePawnDeath(AController* Controller);

protected:

	void RespawnPlayer(AController* Controller);
	virtual void Logout(AController* Exiting) override;

	UPROPERTY(EditDefaultsOnly, Category = "Respawn")
	float RespawnDelay = 2.f;
	
	TMap<TWeakObjectPtr<AController>, FTimerHandle> RespawnTimers;
};
