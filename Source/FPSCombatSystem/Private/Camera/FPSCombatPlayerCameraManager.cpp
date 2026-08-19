// Fill out your copyright notice in the Description page of Project Settings.


#include "Camera/FPSCombatPlayerCameraManager.h"

AFPSCombatPlayerCameraManager::AFPSCombatPlayerCameraManager(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	DefaultFOV = COMBAT_CAMERA_DEFAULT_FOV;
	ViewPitchMin = COMBAT_CAMERA_PITCH_MIN;
	ViewPitchMax = COMBAT_CAMERA_PITCH_MAX;
}
