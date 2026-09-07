// Fill out your copyright notice in the Description page of Project Settings.

#include "Interaction/WorldPickupActor.h"

#include "Equipment/FPSCombatEquipmentManager.h"

AWorldPickupActor::AWorldPickupActor()
{
	bReplicates = true;

	CollisionComp = CreateDefaultSubobject<USphereComponent>(FName("CollisionComponent"));
	SetRootComponent(CollisionComp);
	CollisionComp->InitSphereRadius(75.f);
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionComp->SetCollisionProfileName(FName("OverlapAllDynamic"));

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(FName("MeshComponent"));
	MeshComponent->SetupAttachment(CollisionComp);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AWorldPickupActor::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	UE_LOG(LogTemp, Warning, TEXT("WorldPickupActor::NotifyActorBeginOverlap"));
	if (!HasAuthority() || bIsPicked || !OtherActor) return;

	UE_LOG(LogTemp, Warning, TEXT("WorldPickupActor::NotifyActorBeginOverlap. After HasAuthority"));

	UFPSCombatEquipmentManager* Manager = OtherActor->FindComponentByClass<UFPSCombatEquipmentManager>();
	//UFPSCombatItemManagerComponent* Manager = OtherActor->FindComponentByClass<UFPSCombatItemManagerComponent>();
	if (!Manager) return;

	UE_LOG(LogTemp, Warning, TEXT("WorldPickupActor::NotifyActorBeginOverlap. After ManagerComponent"));
	
	bIsPicked = true;

	for (const FActorItemDefinition& Definitions : PickupInfo.ItemDefinitions)
	{
		Manager->OnEquipItem(Definitions.ItemDefinition);
	}
	// TODO: make a Slot logic
	// TScriptInterface<IInteractionInterface> Interface(this);
	// UFPSCombatInteractionMatching::AddInteractionToInventory(Manager, Interface);

	Destroy();
}
