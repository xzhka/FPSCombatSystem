// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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
	
	virtual void SpawnedActors(const TArray<FFPSCombatEquipmentSpawnActor>& SpawnActors);
	virtual void ClearActors();

	UFUNCTION(BlueprintPure)
	APawn* GetPawn() const;
	
	UFUNCTION(BlueprintPure)
	FORCEINLINE TArray<AActor*> GetActorsToSpawn() const { return ActorsToSpawn; }
	
private:

	UPROPERTY(Replicated)
	TArray<TObjectPtr<AActor>> ActorsToSpawn;
	
};
