// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Projectile/FPSCombatThrowableInstance.h"

UFPSCombatThrowableDefinition* UFPSCombatThrowableInstance::GetThrowableDefinition() const
{
	if (GetDefinition())
	{
		return Cast<UFPSCombatThrowableDefinition>(GetDefinition());
	}
	return nullptr;
}
