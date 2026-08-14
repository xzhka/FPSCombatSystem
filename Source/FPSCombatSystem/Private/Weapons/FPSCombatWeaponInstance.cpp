// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/FPSCombatWeaponInstance.h"


void UFPSCombatWeaponInstance::OnEquipped()
{
	Super::OnEquipped();

	UWorld* World = GetWorld();

	check(World);
	TimeEquipped = World->GetTimeSeconds();
	
}

void UFPSCombatWeaponInstance::OnUnequipped()
{
	Super::OnUnequipped();
}

void UFPSCombatWeaponInstance::UpdateLastFireTime()
{	
	UWorld* World = GetWorld();
	check(World);

	TimeFired = World->GetTimeSeconds();
}

float UFPSCombatWeaponInstance::GetTimeFromLastInteraction() const
{
	UWorld* World = GetWorld();
	check(World);

	const double CurrentTime = World->GetTimeSeconds();
	
	double Result = CurrentTime - TimeEquipped;

	if (Result > 0.f)
	{
		const double FireTime = CurrentTime - TimeFired;
		Result = FMath::Min(Result, FireTime);
	}
	

	return Result;
	
}

TSubclassOf<UAnimInstance> UFPSCombatWeaponInstance::PickAnimLayer(bool bIsEquipped,
	const FGameplayTagContainer& CosmeticTag) const
{
	const FFPSCombatLayerSelectionSet SelectionSet = (bIsEquipped ? EquippedAnimSet : UnequippedAnimSet);
	return SelectionSet.SelectLayers(CosmeticTag);
}
