// FPS Combat project

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "Items/FPSCombatItemDefinition.h"
#include "WorldPickupActor.generated.h"


/*	AWorldPickupActor
 *	
 *	Parent class for world spawnable actors
 */
UCLASS(Abstract)
class FPSCOMBATSYSTEM_API AWorldPickupActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AWorldPickupActor();

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Pickup")
	FGameplayTag StatTagToAdd;

	UPROPERTY(EditDefaultsOnly, Category = "Pickup")
	FGameplayTag MaxAmountToAdd;

	bool TryTopUpStat(UFPSCombatItemInstance* ItemInstance) const;
	
	virtual bool TryGivePickup(APawn* PickupPawn) PURE_VIRTUAL(AWorldPickupActor::TryGivePickup, return false; ); 
	
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);
	
	UPROPERTY(EditDefaultsOnly, Category = "Slots")
	TSubclassOf<UFPSCombatItemDefinition> ItemDefinition;

	UPROPERTY(EditDefaultsOnly, Category = "Pickup")
	TObjectPtr<USphereComponent> CollisionComp;
	
	UPROPERTY(EditDefaultsOnly, Category = "Pickup")
	TObjectPtr<UStaticMeshComponent> MeshComponent; 
	
private:
	void AttemptToPickupWeapon(APawn* PickupPawn);
	
	bool bIsPicked = false;
	
};
