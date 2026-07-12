
#include "FPSCombatGameplayTags.h"

namespace FPSCombatGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG(State_Death, "State.Death");
	UE_DEFINE_GAMEPLAY_TAG(State_Death_Started, "State.Death.Start");
	UE_DEFINE_GAMEPLAY_TAG(State_Death_Ended, "State.Death.Ended");
	
	UE_DEFINE_GAMEPLAY_TAG(State_Stamina_OutOfStamina, "State.Stamina.OutOfStamina");

	UE_DEFINE_GAMEPLAY_TAG(State_Moving_Walking, "State.Moving.Walking");
	UE_DEFINE_GAMEPLAY_TAG(State_Moving_MovingForward, "State.Moving.MovingForward");

	
	UE_DEFINE_GAMEPLAY_TAG(State_Moving_Sprinting, "State.Moving.Sprinting");
	UE_DEFINE_GAMEPLAY_TAG(State_Moving_Dash, "State.Moving.Dash");

	UE_DEFINE_GAMEPLAY_TAG(State_Moving_Airborne, "State.Moving.Airborne");
	UE_DEFINE_GAMEPLAY_TAG(State_Moving_AirborneSource, "State.Moving.AirborneSource");
	UE_DEFINE_GAMEPLAY_TAG(State_Moving_AirborneSource_Updraft, "State.Moving.AirborneSource.Updraft");
	
	UE_DEFINE_GAMEPLAY_TAG(State_Stamina_SprintExhausted, "State.Stamina.SprintExhausted");
	UE_DEFINE_GAMEPLAY_TAG(State_Stamina_DashExhausted, "State.Stamina.DashExhausted");
	UE_DEFINE_GAMEPLAY_TAG(State_Stamina_UpdraftExhausted, "State.Stamina.UpdraftExhausted");
	
	
	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_Cooldown_Duration, "SetByCaller.Cooldown.Duration");
	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_Dash_Duration, "SetByCaller.Dash.Duration");
	
	UE_DEFINE_GAMEPLAY_TAG(Ability_RequiresStamina, "Ability.RequiresStamina");
}

