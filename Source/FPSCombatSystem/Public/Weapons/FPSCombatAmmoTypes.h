#pragma once

#include "CoreMinimal.h"
#include "FPSCombatAmmoTypes.generated.h"


USTRUCT(BlueprintType)
struct FFPSCombatAmmoChangedMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	int32 CurrentAmmo = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 ReserveAmmo = 0;
};
