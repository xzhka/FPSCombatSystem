// FPS Combat project


#include "Interaction/StatsPickupActor.h"

#include "GameModes/FPSCombatPlayerState.h"
#include "Items/FPSCombatItemManagerComponent.h"

bool AStatsPickupActor::TryGivePickup(APawn* PickupPawn)
{
	check(PickupPawn != nullptr);

	if (AFPSCombatPlayerState* PlayerState = PickupPawn->GetPlayerState<AFPSCombatPlayerState>())
	{
		if (UFPSCombatItemManagerComponent* ItemManager = PlayerState->FindComponentByClass<UFPSCombatItemManagerComponent>())
		{
			return TryTopUpStat(ItemManager->FindInstanceByDef(ItemDefinition));
		}
	}
	return false;
}
