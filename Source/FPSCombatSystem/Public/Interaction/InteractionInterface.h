// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Equipment/FPSCombatEquipmentDefinition.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Items/FPSCombatItemInstance.h"
#include "Items/FPSCombatItemManagerComponent.h"
#include "UObject/Interface.h"
#include "InteractionInterface.generated.h"

USTRUCT(BlueprintType)
struct FActorItemInstance
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere, Instanced)
	TObjectPtr<UFPSCombatItemInstance> PreservedInstance = nullptr;
};

USTRUCT(BlueprintType)
struct FActorItemDefinition
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere)
	FGameplayTag StatTag;
	
	UPROPERTY(EditAnywhere)
	int32 StackCount = 1;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UFPSCombatEquipmentDefinition> ItemDefinition = nullptr;
};
		

USTRUCT(BlueprintType)
struct FActorPickupInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TArray<FActorItemInstance> ItemInstances;

	UPROPERTY(EditAnywhere)
	TArray<FActorItemDefinition> ItemDefinitions;
};

UINTERFACE(MinimalAPI, BlueprintType)
class UInteractionInterface : public UInterface
{
	GENERATED_BODY()
};


class FPSCOMBATSYSTEM_API IInteractionInterface
{
	GENERATED_BODY()

public:

	UFUNCTION()
	virtual FActorPickupInfo GetPickupInfo() const = 0;
};


UCLASS()
class FPSCOMBATSYSTEM_API UFPSCombatInteractionMatching : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	static TScriptInterface<IInteractionInterface> GetInteractionInterfaceByActor(AActor* Actor);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly , Category = "Interaction")
	static void AddInteractionToInventory(UFPSCombatItemManagerComponent* Manager, TScriptInterface<IInteractionInterface> Interface);
	
};

