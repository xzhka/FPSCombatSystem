// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "FPSCombatSystem/FPSCombatTeamTypes.h"
#include "GameFramework/PlayerStart.h"
#include "FPSCombatPlayerStart.generated.h"

enum class EFPSCombatTeam : uint8;
/**
 * 
 */
UCLASS()
class FPSCOMBATSYSTEM_API AFPSCombatPlayerStart : public APlayerStart
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
	EFPSCombatTeam TeamType = EFPSCombatTeam::Player;

	FGenericTeamId GetTeamId() const { return FGenericTeamId(static_cast<uint8>(TeamType)); }
};
