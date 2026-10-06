// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FPSCombatGameMode.generated.h"

/**
 * 
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
