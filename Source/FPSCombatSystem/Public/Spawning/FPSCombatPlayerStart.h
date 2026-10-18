// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "FPSCombatPlayerStart.generated.h"

UENUM(BlueprintType)
enum class EFPSCombatSpawnFaction : uint8
{
	Player,
	Enemy
};

/**
 * 
 */
UCLASS()
class FPSCOMBATSYSTEM_API AFPSCombatPlayerStart : public APlayerStart
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
	EFPSCombatSpawnFaction Faction = EFPSCombatSpawnFaction::Player;
};
