// Fill out your copyright notice in the Description page of Project Settings.

#include "Interaction/WorldPickupActor.h"

#include "Equipment/FPSCombatEquipmentManager.h"
#include "Equipment/FPSCombatQuickBarComponent.h"
#include "FPSCombatSystem/FPSCombatGameplayTags.h"

AWorldPickupActor::AWorldPickupActor()
{
	bReplicates = true;

	CollisionComp = CreateDefaultSubobject<USphereComponent>(FName("CollisionComponent"));
	SetRootComponent(CollisionComp);
	CollisionComp->InitSphereRadius(75.f);
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionComp->SetCollisionProfileName(FName("OverlapAllDynamic"));
	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AWorldPickupActor::OnBeginOverlap);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(FName("MeshComponent"));
	MeshComponent->SetupAttachment(CollisionComp);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

bool AWorldPickupActor::GiveWeapon(TSubclassOf<UFPSCombatItemDefinition> Definition, APawn* PickupPawn)
{
	check(PickupPawn != nullptr);
	
	if (AController* Controller = PickupPawn->GetController())
	{
		UFPSCombatQuickBarComponent* QuickBar = Controller->GetComponentByClass<UFPSCombatQuickBarComponent>();
		UFPSCombatItemManagerComponent* ItemManager = PickupPawn->GetComponentByClass<UFPSCombatItemManagerComponent>();
		if (ItemManager != nullptr && QuickBar != nullptr)
		{
			UFPSCombatItemInstance* EquippedInstance = ItemManager->FindInstanceByDef(Definition);
			if (IsValid(EquippedInstance))
			{
				int32 RemainingAmmo = GetDefaultStatValueByTag(Definition, FPSCombatGameplayTags::Data_Weapon_SpareAmmo) - EquippedInstance->GetStack(FPSCombatGameplayTags::Data_Weapon_SpareAmmo);
				if (RemainingAmmo > 0)
				{
					EquippedInstance->AddStackCount(FPSCombatGameplayTags::Data_Weapon_SpareAmmo, RemainingAmmo);
					return true;
				}
			}
			else
			{
				const int32 FreeSlot = QuickBar->GetNextFreeItemSlot();
				if (FreeSlot == INDEX_NONE) return false;
				if (UFPSCombatItemInstance* ItemInstance = ItemManager->AddStack(Definition, FPSCombatGameplayTags::Item_Stat_Quantity, 1))
				{
					QuickBar->AddItemToSlot(ItemInstance, FreeSlot);
					QuickBar->SetActiveSlot(FreeSlot);
					
					return true;
				}
			}
		}
	}
	return false;
}

void AWorldPickupActor::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                       UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APawn* PickupPawn = Cast<APawn>(OtherActor);
	if (GetLocalRole() == ROLE_Authority && PickupPawn != nullptr)
	{
		AttemptToPickupWeapon(PickupPawn);
	}
}

void AWorldPickupActor::AttemptToPickupWeapon(APawn* PickupPawn)
{
	if (GetLocalRole() == ROLE_Authority && PickupPawn != nullptr)
	{

		if (EquipmentDefinition == nullptr) return;

		bIsPicked = true;
		
		if (GiveWeapon(EquipmentDefinition, PickupPawn))
		{
			Destroy();
		}
		else
		{
			bIsPicked = false;
		}
	}
}

int32 AWorldPickupActor::GetDefaultStatValueByTag(TSubclassOf<UFPSCombatItemDefinition> ItemClass,
	FGameplayTag Tag) const
{
	if (ItemClass != nullptr)
	{
		if (UFPSCombatItemInstance* ItemInstance = ItemClass->GetDefaultObject<UFPSCombatItemInstance>())
		{
			if (const UFPSCombatItemFragment_Stats* FragmentStats = Cast<UFPSCombatItemFragment_Stats>(ItemInstance->FindFragmentByType(UFPSCombatItemFragment_Stats::StaticClass())))
			{
				return FragmentStats->GetStatsByTag(Tag);
			}
		}
	}
	return 0;
}