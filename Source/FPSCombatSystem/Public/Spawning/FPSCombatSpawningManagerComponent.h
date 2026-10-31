// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/GameStateComponent.h"
#include "FPSCombatSpawningManagerComponent.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, meta=(BlueprintSpawnableComponent))
class FPSCOMBATSYSTEM_API UFPSCombatSpawningManagerComponent : public UGameStateComponent
{
	GENERATED_BODY()

public:
	AActor* ChoosePlayerStart(AController* InController) const;

private:

	const APawn* GetPawnToFit(AController* Controller) const;
	bool IsBlockedStart(const APawn* ActorToFit, AActor* Start) const;
	
};
