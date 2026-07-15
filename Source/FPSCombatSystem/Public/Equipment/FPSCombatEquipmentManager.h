// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FPSCombatEquipmentDefinition.h"
#include "FPSCombatEquipmentInstance.h"
#include "AbilitySystem/FPSCombatAbilitySet.h"
#include "Components/PawnComponent.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "FPSCombatEquipmentManager.generated.h"

USTRUCT(BlueprintType)
struct FFPSCombatAppliedEquipmentList : public FFastArraySerializer
{
	GENERATED_BODY()
	FFPSCombatAppliedEquipmentList() {}

	
protected:

	UPROPERTY()
	TObjectPtr<UFPSCombatEquipmentInstance> Instance = nullptr; 

	UPROPERTY()
	TSoftObjectPtr<UFPSCombatEquipmentDefinition> Definition;

	UPROPERTY(NotReplicated)
	FCombatAbilitySet_GrantedHandles GrantedHandles;
};

USTRUCT(BlueprintType)
struct FFPSCombatEquipmentList : public FFastArraySerializer
{

	GENERATED_BODY()

	FFPSCombatEquipmentList() {}

public:

	UFPSCombatEquipmentInstance* AddEntry(TSoftObjectPtr<UFPSCombatEquipmentDefinition> Definition);
	void RemoveEntry(UFPSCombatEquipmentInstance* EquipmentInstance);
	
	
private:

	UPROPERTY()
	TArray<FFPSCombatAppliedEquipmentList> EquipmentList;
	
	UPROPERTY(NotReplicated)
	TObjectPtr<UActorComponent> OwnerComponent;
};




UCLASS()
class FPSCOMBATSYSTEM_API UFPSCombatEquipmentManager : public UPawnComponent
{
	GENERATED_BODY()





private:
	UPROPERTY(Replicated)
	FFPSCombatEquipmentList EquipmentList;
};
