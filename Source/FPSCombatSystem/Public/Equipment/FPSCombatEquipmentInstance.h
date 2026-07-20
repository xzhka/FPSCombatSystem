// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FPSCombatEquipmentDefinition.h"
#include "FPSCombatEquipmentInstance.generated.h"

struct FFPSCombatEquipmentSpawnActor;
class APawn;


/**
 * 
 */
UCLASS()
class FPSCOMBATSYSTEM_API UFPSCombatEquipmentInstance : public UObject
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void OnEquipped();
	virtual void OnUnequipped();
	
	virtual void SpawnEquipmentActors(const TArray<FFPSCombatEquipmentSpawnActor>& SpawnActors);
	virtual void ClearEquipmentActors();

	
	UFUNCTION(BlueprintPure)
	APawn* GetPawn() const;


	UFUNCTION(BlueprintPure, BlueprintCallable)
	FORCEINLINE UFPSCombatEquipmentDefinition* GetDefinition() const { return InstanceDefinition; }
	
	UFUNCTION(BlueprintPure)
	FORCEINLINE TArray<AActor*> GetActorsToSpawn() const { return ActorsToSpawn; }
	
private:

	UPROPERTY()
	TObjectPtr<class UFPSCombatEquipmentDefinition> InstanceDefinition;
	
	UPROPERTY(Replicated)
	TArray<TObjectPtr<AActor>> ActorsToSpawn;
	
};
