// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipment/FPSCombatEquipmentInstance.h"
#include "Equipment/FPSCombatEquipmentDefinition.h"
#include "Iris/ReplicationSystem/ReplicationFragmentUtil.h"
#include "GameFramework/Character.h"

#include "Net/UnrealNetwork.h"


void UFPSCombatEquipmentInstance::SpawnEquipmentActors(const TArray<FFPSCombatEquipmentSpawnActor>& SpawnActors)
{
	if (APawn* OwningPawn = GetPawn())
	{
		USceneComponent* RootComp = OwningPawn->GetRootComponent();
		if (const ACharacter* Char = Cast<ACharacter>(OwningPawn))
		{
			RootComp = Char->GetMesh();
		}
		
		for (const FFPSCombatEquipmentSpawnActor& SpawnActorInfo : SpawnActors)
		{
			AActor* OwningActor = GetWorld()->SpawnActorDeferred<AActor>(SpawnActorInfo.SpawnActorClass, FTransform::Identity, OwningPawn);
			if (OwningActor)
			{
				OwningActor->FinishSpawning(FTransform::Identity, true);
				OwningActor->AttachToComponent(RootComp, FAttachmentTransformRules::KeepRelativeTransform, SpawnActorInfo.AttachSocket);
				
				ActorsToSpawn.Add(OwningActor);
			}
		}
	}
}

void UFPSCombatEquipmentInstance::ClearEquipmentActors()
{	
	for (AActor* Actor : ActorsToSpawn)
	{
		if (Actor)
		{
			Actor->Destroy();
		}
	}
}

APawn* UFPSCombatEquipmentInstance::GetPawn() const
{
	return Cast<APawn>(GetOuter());
}

void UFPSCombatEquipmentInstance::RegisterReplicationFragments(UE::Net::FFragmentRegistrationContext& Context,
	UE::Net::EFragmentRegistrationFlags RegistrationFlags)
{
	using namespace UE::Net;

	FReplicationFragmentUtil::CreateAndRegisterFragmentsForObject(this, Context, RegistrationFlags);
}

void UFPSCombatEquipmentInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	UObject::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UFPSCombatEquipmentInstance, ActorsToSpawn);
	DOREPLIFETIME(UFPSCombatEquipmentInstance, InstanceDefinition);
}

void UFPSCombatEquipmentInstance::OnEquipped()
{
}

void UFPSCombatEquipmentInstance::OnUnequipped()
{
}
