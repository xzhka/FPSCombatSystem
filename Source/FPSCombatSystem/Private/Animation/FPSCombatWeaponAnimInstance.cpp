// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/FPSCombatWeaponAnimInstance.h"

#include "Characters/FPSCombatCharacter.h"
#include "Engine/SkeletalMeshSocket.h"

void UFPSCombatWeaponAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	MagAlpha = FMath::FInterpTo(MagAlpha, bMagInHand ? 1.f : 0.f, DeltaSeconds, MagBlendSpeed);

	const AFPSCombatWeaponActor* WeaponActor = Cast<AFPSCombatWeaponActor>(GetOwningActor());
	if (!WeaponActor) return;
	
	const AFPSCombatCharacter* Character = Cast<AFPSCombatCharacter>(WeaponActor->GetAttachParentActor());
	if (!Character) return;

	const USkeletalMeshComponent* CharacterMesh = Character->GetMesh();
	const USkeletalMeshComponent* WeaponMesh = WeaponActor->GetWeaponMesh();
	if (!CharacterMesh && !WeaponMesh) return;

	FTransform SocketOffset = FTransform::Identity;
	if (const USkeletalMeshSocket* Socket = WeaponMesh->GetSocketByName(WeaponMagSocketName))
	{
		SocketOffset = Socket->GetSocketLocalTransform().Inverse();
	}
	
	MagTarget = SocketOffset * CharacterMesh->GetSocketTransform(MagGripSocketName);
}
