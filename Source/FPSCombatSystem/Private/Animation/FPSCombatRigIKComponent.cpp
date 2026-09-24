// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/FPSCombatRigIKComponent.h"

#include "Equipment/FPSCombatEquipmentInstance.h"
#include "FPSCombatSystem/FPSCombatGameplayTags.h"
#include "GameFramework/Character.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Weapons/FPSCombatWeaponDefinition.h"

UFPSCombatRigIKComponent::UFPSCombatRigIKComponent()
{
	PrimaryComponentTick.bCanEverTick = true;	
}

void UFPSCombatRigIKComponent::BeginPlay()
{
	Super::BeginPlay();
	UGameplayMessageSubsystem::Get(this).RegisterListener(FPSCombatGameplayTags::Message_Equipment_Change, this, &UFPSCombatRigIKComponent::OnEquipmentChange);

	if (ACharacter* Character = Cast<ACharacter>(GetOwner()))
	{
		AddTickPrerequisiteComponent(Character->GetMesh());	
	}
}

void UFPSCombatRigIKComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!CachedWeaponActor) return;

	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (!Character || !Character->GetMesh()) return;
	
	const FTransform SocketWorld = CachedWeaponActor->GetSocketTransform(CachedSocketName);
	const FTransform GoalComponentSpace = SocketWorld.GetRelativeTransform(Character->GetMesh()->GetComponentTransform());
	SetIKRigGoalPositionAndRotation(LeftHandGoalName, GoalComponentSpace.GetLocation(), GoalComponentSpace.GetRotation(), 1.f, 1.f);
}

void UFPSCombatRigIKComponent::OnEquipmentChange(FGameplayTag Channel, const FFPSCombatEquipmentChangedMessage& Message)
{
	if (!Message.NewObjectInstance || Message.NewObjectInstance->GetPawn() != GetOwner())
	{
		return;
	}

	UFPSCombatWeaponDefinition* WeaponDef = Message.bIsEquipped ? Cast<UFPSCombatWeaponDefinition>(Message.NewObjectInstance->GetDefinition()) : nullptr;

	if (!WeaponDef || WeaponDef->LeftHandGripSocket.IsNone())
	{
		CachedWeaponActor = nullptr;
		SetIKRigGoalPositionAndRotation(LeftHandGoalName, FVector::ZeroVector, FQuat::Identity, 0.f, 0.f);
		return;
	}

	for (AActor* SpawnedActor : Message.NewObjectInstance->GetActorsToSpawn())
	{
		if (AFPSCombatWeaponActor* WeaponActor = Cast<AFPSCombatWeaponActor>(SpawnedActor))
		{
			CachedWeaponActor = WeaponActor;
			CachedSocketName = WeaponDef->LeftHandGripSocket;
		}
	}
}
