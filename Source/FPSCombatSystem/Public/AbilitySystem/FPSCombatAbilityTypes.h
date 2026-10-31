// FPS Combat project

#pragma once

#include "CoreMinimal.h"
#include "FPSCombatAbilityTypes.generated.h"

/* Enum used to represent abilities activation behaviour */
UENUM(BlueprintType)
enum class EFPSCombatAbilityActivationPolicy : uint8
{
	OnInputTriggered,
	WhileInputActive,
	OnSpawn
};