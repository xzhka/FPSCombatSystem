// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "PhysicMaterialWithTag.generated.h"

/**
 * 
 */
UCLASS()
class FPSCOMBATSYSTEM_API UPhysicMaterialWithTag : public UPhysicalMaterial
{
	GENERATED_BODY()

public:
	UPhysicMaterialWithTag (const FObjectInitializer& ObjectInitializer);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Physics")
	FGameplayTagContainer Tags;
};