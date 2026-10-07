// FPS Combat project


#include "Weapons/FPSCombatWeaponActor.h"


AFPSCombatWeaponActor::AFPSCombatWeaponActor()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	WeaponMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon Mesh Component"));
	WeaponMeshComponent->SetupAttachment(RootComponent);
	WeaponMeshComponent->SetFirstPersonPrimitiveType(EFirstPersonPrimitiveType::FirstPerson);
}

FTransform AFPSCombatWeaponActor::GetSocketTransform(FName SocketName) const
{
	if (WeaponMeshComponent && WeaponMeshComponent->DoesSocketExist(SocketName))
	{
		return WeaponMeshComponent->GetSocketTransform(SocketName);
	}
	return GetActorTransform();
}

