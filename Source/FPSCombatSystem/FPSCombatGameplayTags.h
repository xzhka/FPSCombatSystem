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
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Moving_Airborne);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Moving_Sprinting);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Moving_Dash);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_QuickBar_ChangeSlot);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Moving_AirborneSource);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Moving_AirborneSource_Updraft);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Projectile_Throw);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Throwable_Release);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Stamina_SprintExhausted);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Stamina_DashExhausted);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Stamina_UpdraftExhausted);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Weapon_Fire);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Weapon_Aiming);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Weapon_Reload);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Ability_Updraft);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data_Tags_OutOfAmmo);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data_Weapon_SpareAmmo);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data_Weapon_Ammo_Mag);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Stat_Quantity);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_QuickBar_Next);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_QuickBar_Previous);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_QuickBar_Select1);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_QuickBar_Select2);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_QuickBar_Select3);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Cooldown_Duration);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Dash_Duration);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Throw_Duration);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Data_Damage);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Data_Heal);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Message_Equipment_Change);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Message_Item_StackChange);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Message_Item_Added);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Message_Item_Removed);

	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_RequiresStamina);
	
}