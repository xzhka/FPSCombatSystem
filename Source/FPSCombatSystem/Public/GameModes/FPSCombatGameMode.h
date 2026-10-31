// FPS Combat project

#pragma once

#include "CoreMinimal.h"
#include "Characters/FPSCombatPawnData.h"
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
	UFUNCTION(BlueprintCallable, Category = "Combat|Data")
	const UFPSCombatPawnData* GetPlayerPawnData(const AController* InController) const;
	
	void HandlePawnDeath(AController* Controller);

	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Combat|Data")
	TObjectPtr<const UFPSCombatPawnData> DefaultPawnData;

	UPROPERTY(EditDefaultsOnly, Category = "Combat|Data")
	TObjectPtr<const UFPSCombatPawnData> DefaultEnemyData;
		
	void RespawnPlayer(AController* Controller);
	virtual void Logout(AController* Exiting) override;
	
	UPROPERTY(EditDefaultsOnly, Category = "Respawn")
	float RespawnDelay = 2.f;
	
	TMap<TWeakObjectPtr<AController>, FTimerHandle> RespawnTimers;
};
