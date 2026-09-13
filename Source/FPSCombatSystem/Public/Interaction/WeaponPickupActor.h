// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interaction/WorldPickupActor.h"
#include "WeaponPickupActor.generated.h"

/**
 * 
 */
UCLASS()
class FPSCOMBATSYSTEM_API AWeaponPickupActor : public AWorldPickupActor
{
	GENERATED_BODY()

protected:
	virtual bool TryGivePickup(APawn* PickupPawn) override;
};
