// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "FPSCombatPlayerCameraManager.generated.h"

#define COMBAT_CAMERA_DEFAULT_FOV (90.f);
#define COMBAT_CAMERA_PITCH_MIN (-80.f);
#define COMBAT_CAMERA_PITCH_MAX (80.f);


/**
 * 
 */
UCLASS()
class FPSCOMBATSYSTEM_API AFPSCombatPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()

	AFPSCombatPlayerCameraManager(const FObjectInitializer& ObjectInitializer);
	
};
