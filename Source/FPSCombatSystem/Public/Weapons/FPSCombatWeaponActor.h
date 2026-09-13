// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FPSCombatWeaponActor.generated.h"

UCLASS()
class FPSCOMBATSYSTEM_API AFPSCombatWeaponActor : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* WeaponMeshComponent;
	
public:	
	AFPSCombatWeaponActor();


protected:
	UFUNCTION(BlueprintCallable)
	FTransform GetMuzzleLocation() const;
	
	UPROPERTY(EditDefaultsOnly, Category = "Names")
	FName MuzzleSocketName;	
};
