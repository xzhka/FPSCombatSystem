// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/FPSCombatWeaponActor.h"


AFPSCombatWeaponActor::AFPSCombatWeaponActor()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	WeaponMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon Mesh Component"));
	WeaponMeshComponent->SetupAttachment(RootComponent);
	WeaponMeshComponent->SetFirstPersonPrimitiveType(EFirstPersonPrimitiveType::FirstPerson);
}

FTransform AFPSCombatWeaponActor::GetMuzzleLocation() const
{
	if (WeaponMeshComponent && WeaponMeshComponent->DoesSocketExist(MuzzleSocketName))
	{
		return WeaponMeshComponent->GetSocketTransform(MuzzleSocketName);
	}
	return GetActorTransform();
}

