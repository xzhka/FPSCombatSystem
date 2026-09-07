// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/InteractionInterface.h"


TScriptInterface<IInteractionInterface> UFPSCombatInteractionMatching::GetInteractionInterfaceByActor(
	AActor* Actor)
{
	if (!Actor) return nullptr;
	
	TScriptInterface<IInteractionInterface> InteractionInterface(Actor);
	if (InteractionInterface) return InteractionInterface;


	TArray<UActorComponent*> InteractionComponents = Actor->GetComponentsByInterface(UInteractionInterface::StaticClass());
	if (InteractionComponents.Num() > 0)
	{
		return TScriptInterface<IInteractionInterface>(InteractionComponents[0]);
	}

	return TScriptInterface<IInteractionInterface>();
}

void UFPSCombatInteractionMatching::AddInteractionToInventory(UFPSCombatItemManagerComponent* Manager,
	TScriptInterface<IInteractionInterface> Interface)
{
	if (Manager && Interface)
	{
		FActorPickupInfo Pickup = Interface->GetPickupInfo();
		
		for (const FActorItemDefinition& Definition : Pickup.ItemDefinitions)
		{
			
			Manager->AddStack(Definition.ItemDefinition, Definition.StatTag, Definition.StackCount);
		}
		
		for (const FActorItemInstance& Instance : Pickup.ItemInstances)
		{
			Manager->AddInstance(Instance.PreservedInstance);
		}
	}
}
