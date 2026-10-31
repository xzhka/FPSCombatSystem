// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "FPSCombatSystem/FPSCombatTeamTypes.h"
#include "FPSCombatPawnData.generated.h"

class UFPSCombatAbilitySet;
/**
 * 
 */
UCLASS(BlueprintType, Const)
class FPSCOMBATSYSTEM_API UFPSCombatPawnData : public UDataAsset
{
	GENERATED_BODY()
public:
	UFPSCombatPawnData(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Pawn")
	TSubclassOf<APawn> PawnClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Sets")
	TArray<TObjectPtr<UFPSCombatAbilitySet>> AbilitySets;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Team")
	EFPSCombatTeam Team;
};
