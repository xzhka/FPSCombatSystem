// FPS Combat project


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
		// Character attach weapon right to the skeletal mesh (the socket
		// lives here).
		if (const ACharacter* Char = Cast<ACharacter>(OwningPawn))
		{
			RootComp = Char->GetMesh();
		}
		
		for (const FFPSCombatEquipmentSpawnActor& SpawnActorInfo : SpawnActors)
		{
			// Spawn at identity, then attach and apply the relative transform
			// from the definition.
			AActor* OwningActor = GetWorld()->SpawnActorDeferred<AActor>(SpawnActorInfo.SpawnActorClass, FTransform::Identity, OwningPawn);
			if (OwningActor)
			{
				OwningActor->FinishSpawning(FTransform::Identity);
				OwningActor->SetActorRelativeTransform(SpawnActorInfo.ActorTransform);
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
	ActorsToSpawn.Empty();
}

void UFPSCombatEquipmentInstance::SetEquipmentActorsHidden(bool bHidden)
{
	for (AActor* Actor : ActorsToSpawn)
	{
		if (Actor)
		{
			// Hiding also disables collision and ticking, so a hidden item is
			// fully inert.
			Actor->SetActorHiddenInGame(bHidden);
			Actor->SetActorEnableCollision(!bHidden);
			Actor->SetActorTickEnabled(!bHidden);
		}
	}
}

void UFPSCombatEquipmentInstance::SpawnEquipmentActorsFromInstance()
{
	// Spawn from the definition only if nothing is spawned yet.
	if (ActorsToSpawn.Num() > 0)
	{
		return;
	}

	if (const UFPSCombatEquipmentDefinition* DefCDO = GetDefinition())
	{
		SpawnEquipmentActors(DefCDO->SpawnActors);
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
	DOREPLIFETIME(UFPSCombatEquipmentInstance, Instance);
}

void UFPSCombatEquipmentInstance::OnEquipped()
{
	K2_OnEquipped();
}

void UFPSCombatEquipmentInstance::OnUnequipped()
{
	K2_OnUnequipped();
}
