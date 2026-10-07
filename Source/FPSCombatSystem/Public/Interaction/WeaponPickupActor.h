// FPS Combat project

#pragma once

#include "CoreMinimal.h"
#include "Interaction/WorldPickupActor.h"
#include "WeaponPickupActor.generated.h"

/*	AWeaponPickupActor
 *	
 *	Derived representation of weapon actors
 */
UCLASS()
class FPSCOMBATSYSTEM_API AWeaponPickupActor : public AWorldPickupActor
{
	GENERATED_BODY()

protected:
	virtual bool TryGivePickup(APawn* PickupPawn) override;
};
