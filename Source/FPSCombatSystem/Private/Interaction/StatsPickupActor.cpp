// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/StatsPickupActor.h"

#include "Items/FPSCombatItemManagerComponent.h"

bool AStatsPickupActor::TryGivePickup(APawn* PickupPawn)
{
	check(PickupPawn != nullptr);

	if (UFPSCombatItemManagerComponent* ItemManager = PickupPawn->FindComponentByClass<UFPSCombatItemManagerComponent>())
	{
		return TryTopUpStat(ItemManager->FindInstanceByDef(ItemDefinition));
	}
	return false;
}
