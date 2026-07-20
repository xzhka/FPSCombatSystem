// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/FPSCombatAbilitySet.h"
#include "Engine/DataAsset.h"
#include "FPSCombatEquipmentDefinition.generated.h"

class UFPSCombatEquipmentInstance;

USTRUCT()
struct FFPSCombatEquipmentSpawnActor
{
	GENERATED_BODY()

	FFPSCombatEquipmentSpawnActor() { }
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> SpawnActorClass;

	UPROPERTY(EditAnywhere)
	FName SpawnActorName;
};


UCLASS()
class FPSCOMBATSYSTEM_API UFPSCombatEquipmentDefinition : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category="Equipment")
	TSubclassOf<UFPSCombatEquipmentInstance> ActorEquipmentClass;

	UPROPERTY(EditDefaultsOnly, Category="Equipment")
	TArray<TObjectPtr<const UFPSCombatAbilitySet>> AbilitySet;

	UPROPERTY(EditDefaultsOnly, Category="Equipment")
	TArray<FFPSCombatEquipmentSpawnActor> SpawnActors;
	
};
