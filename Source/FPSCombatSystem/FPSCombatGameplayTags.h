#pragma once
#include "NativeGameplayTags.h"


namespace FPSCombatGameplayTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Death);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Death_Started);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Death_Ended);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Stamina_OutOfStamina);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Moving_Walking);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Moving_MovingForward);
	
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Moving_Sprinting);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Moving_Dash);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Moving_Airborne);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Moving_AirborneSource);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Moving_AirborneSource_Updraft);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Stamina_SprintExhausted);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Stamina_DashExhausted);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Stamina_UpdraftExhausted);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Cooldown_Duration);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Dash_Duration);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_RequiresStamina);
	
}