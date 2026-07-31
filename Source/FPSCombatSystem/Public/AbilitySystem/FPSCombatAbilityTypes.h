#pragma once

#include "CoreMinimal.h"
#include "FPSCombatAbilityTypes.generated.h"


UENUM(BlueprintType)
enum class EFPSCombatAbilityActivationPolicy : uint8
{
	OnInputTriggered,
	WhileInputActive,
	OnSpawn
};