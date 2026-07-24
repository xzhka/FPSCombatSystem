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
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Moving_Sprinting);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Moving_Dash);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Moving_Airborne);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Moving_AirborneSource);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Moving_AirborneSource_Updraft);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Stamina_SprintExhausted);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Stamina_DashExhausted);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Stamina_UpdraftExhausted);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Weapon_Fire);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Weapon_Aiming);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Weapon_Reload);
	
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data_Tags_OutOfAmmo);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Cooldown_Duration);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Dash_Duration);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Data_Damage);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Data_Heal);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Message_Ammo_Change);

	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_RequiresStamina);
	
}