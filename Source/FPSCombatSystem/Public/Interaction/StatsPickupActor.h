// FPS Combat project

#pragma once

#include "CoreMinimal.h"
#include "Interaction/WorldPickupActor.h"
#include "StatsPickupActor.generated.h"

/*	AWeaponPickupActor
 *	
 *	Derived representation of stat actors
 */
UCLASS()
class FPSCOMBATSYSTEM_API AStatsPickupActor : public AWorldPickupActor
{
	GENERATED_BODY()
protected:
	virtual bool TryGivePickup(APawn* PickupPawn) override;
	
};
