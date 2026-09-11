
#include "FPSCombatGameplayTags.h"

namespace FPSCombatGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG(State_Death, "State.Death");
	UE_DEFINE_GAMEPLAY_TAG(State_Death_Started, "State.Death.Start");
	UE_DEFINE_GAMEPLAY_TAG(State_Death_Ended, "State.Death.Ended");
	
	UE_DEFINE_GAMEPLAY_TAG(State_Stamina_OutOfStamina, "State.Stamina.OutOfStamina");

	UE_DEFINE_GAMEPLAY_TAG(State_Moving_Walking, "State.Moving.Walking");
	UE_DEFINE_GAMEPLAY_TAG(State_Moving_MovingForward, "State.Moving.MovingForward");
	UE_DEFINE_GAMEPLAY_TAG(State_Moving_Airborne, "State.Moving.Airborne");
	
	UE_DEFINE_GAMEPLAY_TAG(Ability_Moving_Sprinting, "Ability.Moving.Sprinting");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Moving_Dash, "Ability.Moving.Dash");
	
	UE_DEFINE_GAMEPLAY_TAG(Ability_QuickBar_ChangeSlot, "Ability.QuickBar.ChangeSlot");
	
	UE_DEFINE_GAMEPLAY_TAG(Ability_Moving_AirborneSource, "Ability.Moving.AirborneSource");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Moving_AirborneSource_Updraft, "Ability.Moving.AirborneSource.Updraft");
	
	UE_DEFINE_GAMEPLAY_TAG(Ability_Throwable_Release, "Ability.Throwable.Release");
	
	UE_DEFINE_GAMEPLAY_TAG(Ability_Projectile_Throw, "Ability.Projectile.Throw");
	
	UE_DEFINE_GAMEPLAY_TAG(State_Stamina_SprintExhausted, "State.Stamina.SprintExhausted");
	UE_DEFINE_GAMEPLAY_TAG(State_Stamina_DashExhausted, "State.Stamina.DashExhausted");
	UE_DEFINE_GAMEPLAY_TAG(State_Stamina_UpdraftExhausted, "State.Stamina.UpdraftExhausted");

	UE_DEFINE_GAMEPLAY_TAG(Weapon_Fire, "Weapon.Fire");
	UE_DEFINE_GAMEPLAY_TAG(Weapon_Aiming, "Weapon.Aiming");
	UE_DEFINE_GAMEPLAY_TAG(Weapon_Reload, "Weapon.Reload");
	
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Ability_Updraft, "GameplayCue.Ability.Updraft");
	
	UE_DEFINE_GAMEPLAY_TAG(Data_Tags_OutOfAmmo, "Data.Tags.OutOfAmmo");
	UE_DEFINE_GAMEPLAY_TAG(Data_Weapon_SpareAmmo, "Data.Weapon.SpareAmmo");
	UE_DEFINE_GAMEPLAY_TAG(Data_Weapon_Ammo_Mag, "Data.Weapon.Ammo.Mag");
	
	UE_DEFINE_GAMEPLAY_TAG(Item_Stat_Quantity, "Item.Stat.Quantity");
	
	UE_DEFINE_GAMEPLAY_TAG(Input_QuickBar_Next, "Input.QuickBar.Next");
	UE_DEFINE_GAMEPLAY_TAG(Input_QuickBar_Previous, "Input.QuickBar.Previous");
	UE_DEFINE_GAMEPLAY_TAG(Input_QuickBar_Select1, "Input.QuickBar.Select1");
	UE_DEFINE_GAMEPLAY_TAG(Input_QuickBar_Select2, "Input.QuickBar.Select2");
	UE_DEFINE_GAMEPLAY_TAG(Input_QuickBar_Select3, "Input.QuickBar.Select3");
	
	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_Cooldown_Duration, "SetByCaller.Cooldown.Duration");
	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_Dash_Duration, "SetByCaller.Dash.Duration");
	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_Throw_Duration, "SetByCaller.Throw.Duration");
	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_Data_Damage, "SetByCaller.Data.Damage");
	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_Data_Heal, "SetByCaller.Data.Heal");
	
	UE_DEFINE_GAMEPLAY_TAG(Message_Equipment_Change, "Message.Equipment.Change");
	UE_DEFINE_GAMEPLAY_TAG(Message_Item_StackChange, "Message.Item.StackChange");
	UE_DEFINE_GAMEPLAY_TAG(Message_Item_Added, "Message.Item.Added");
	UE_DEFINE_GAMEPLAY_TAG(Message_Item_Removed, "Message.Item.Removed");
	
	UE_DEFINE_GAMEPLAY_TAG(Ability_RequiresStamina, "Ability.RequiresStamina");
}

