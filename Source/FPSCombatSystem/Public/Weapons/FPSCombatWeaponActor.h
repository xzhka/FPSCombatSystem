// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FPSCombatWeaponActor.generated.h"

class UAnimMontage;

UCLASS()
class FPSCOMBATSYSTEM_API AFPSCombatWeaponActor : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* WeaponMeshComponent;
	
public:	
	AFPSCombatWeaponActor();

	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMeshComponent; } 
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Transform")
	FTransform GetSocketTransform(FName SocketName) const;
};
