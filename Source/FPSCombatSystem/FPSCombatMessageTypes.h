#pragma once

#include "CoreMinimal.h"
#include "FPSCombatMessageTypes.generated.h"


class UFPSCombatEquipmentInstance;

USTRUCT(BlueprintType)
struct FFPSCombatAmmoChangedMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	int32 CurrentAmmo = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 ReserveAmmo = 0;
};

USTRUCT(BlueprintType)
struct FFPSCombatEquipmentChangedMessage
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UFPSCombatEquipmentInstance> NewObjectInstance = nullptr;

	UPROPERTY(BlueprintReadOnly)
	bool bIsEquipped = false;
	
};
