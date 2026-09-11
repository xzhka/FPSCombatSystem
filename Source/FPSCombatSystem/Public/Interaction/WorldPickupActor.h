// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractionInterface.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "WorldPickupActor.generated.h"



UCLASS()
class FPSCOMBATSYSTEM_API AWorldPickupActor : public AActor, public IInteractionInterface
{
	GENERATED_BODY()
	
public:	
	AWorldPickupActor();
	
	virtual FActorPickupInfo GetPickupInfo() const override { return PickupInfo; }

	UFUNCTION(BlueprintCallable)
	bool GiveWeapon(TSubclassOf<UFPSCombatItemDefinition> Definition, APawn* PickupPawn);

protected:
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);

	void AttemptToPickupWeapon(APawn* PickupPawn);

	int32 GetDefaultStatValueByTag(TSubclassOf<UFPSCombatItemDefinition> ItemClass, FGameplayTag Tag) const;
	
	UPROPERTY(EditDefaultsOnly, Category = "Slots")
	TSubclassOf<UFPSCombatItemDefinition> EquipmentDefinition;
	
	UPROPERTY(EditDefaultsOnly, Category = "Pickup")	
	FActorPickupInfo PickupInfo;

	UPROPERTY(EditDefaultsOnly, Category = "Pickup")
	TObjectPtr<USphereComponent> CollisionComp;
	
	UPROPERTY(EditDefaultsOnly, Category = "Pickup")
	TObjectPtr<UStaticMeshComponent> MeshComponent; 
	
private:
	bool bIsPicked = false;
	
};
