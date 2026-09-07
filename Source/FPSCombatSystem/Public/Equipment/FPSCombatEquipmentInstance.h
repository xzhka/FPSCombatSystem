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
UCLASS(Blueprintable, BlueprintType)
class FPSCOMBATSYSTEM_API UFPSCombatEquipmentInstance : public UObject
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void OnEquipped();
	virtual void OnUnequipped();
	
	virtual void SpawnEquipmentActors(const TArray<FFPSCombatEquipmentSpawnActor>& SpawnActors);
	virtual void ClearEquipmentActors();

	void SetEquipmentActorsHidden(bool bHidden);
	
	UFUNCTION(BlueprintPure)
	APawn* GetPawn() const;

	void SetDefinition(UFPSCombatEquipmentDefinition* InDefinition) { InstanceDefinition = InDefinition; }
	
	UFUNCTION(BlueprintPure, BlueprintCallable)
	FORCEINLINE UFPSCombatEquipmentDefinition* GetDefinition() const { return InstanceDefinition; }
	
	UFUNCTION(BlueprintPure)
	FORCEINLINE TArray<AActor*> GetActorsToSpawn() const { return ActorsToSpawn; }

	virtual bool IsSupportedForNetworking() const override { return true; }

	virtual void RegisterReplicationFragments(UE::Net::FFragmentRegistrationContext& Context, UE::Net::EFragmentRegistrationFlags RegistrationFlags) override;

	UFUNCTION(BlueprintImplementableEvent, Category =Equipment)
	void K2_OnEquipped();

	UFUNCTION(BlueprintImplementableEvent, Category =Equipment)
	void K2_OnUnequipped();
	
private:

	UPROPERTY(Replicated)
	TObjectPtr<class UFPSCombatEquipmentDefinition> InstanceDefinition;
	
	UPROPERTY(Replicated)
	TArray<TObjectPtr<AActor>> ActorsToSpawn;
	
};
