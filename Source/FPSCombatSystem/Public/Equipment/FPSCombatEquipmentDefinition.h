// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FPSCombatEquipmentInstance.h"
#include "Engine/DataAsset.h"
#include "FPSCombatEquipmentDefinition.generated.h"

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
class FPSCOMBATSYSTEM_API UFPSCombatEquipmentDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category="Equipment")
	TSubclassOf<UFPSCombatEquipmentInstance> ActorEquipmentClass;

	UPROPERTY(EditDefaultsOnly, Category="Equipment")
	TObjectPtr<class UFPSCombatAbilitySet> AbilitySet;

	UPROPERTY(EditDefaultsOnly, Category="Equipment")
	TArray<FFPSCombatEquipmentSpawnActor> SpawnActors;
	
};
