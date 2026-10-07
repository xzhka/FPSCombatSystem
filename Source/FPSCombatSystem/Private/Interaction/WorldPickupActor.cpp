// FPS Combat project

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


bool AWorldPickupActor::TryTopUpStat(UFPSCombatItemInstance* ItemInstance) const
{
	if (!ItemInstance) return false;
	
	const int32 RemainingStat = ItemInstance->GetDefaultStatsByValue(StatTagToAdd) - ItemInstance->GetStack(StatTagToAdd);
	if (RemainingStat > 0 && RemainingStat < ItemInstance->GetDefaultStatsByValue(MaxAmountToAdd))
	{
		ItemInstance->AddStackCount(StatTagToAdd, RemainingStat);
		return true;
	}

	if (RemainingStat >= ItemInstance->GetDefaultStatsByValue(MaxAmountToAdd))
	{
		ItemInstance->AddStackCount(StatTagToAdd, ItemInstance->GetDefaultStatsByValue(MaxAmountToAdd));
		return true;
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

		if (ItemDefinition == nullptr) return;

		bIsPicked = true;
		
		if (TryGivePickup(PickupPawn))
		{
			Destroy();
		}
		else
		{
			bIsPicked = false;
		}
	}
}