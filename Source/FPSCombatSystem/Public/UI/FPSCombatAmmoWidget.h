// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Weapons/FPSCombatAmmoTypes.h"
#include "FPSCombatAmmoWidget.generated.h"

/**
 * 
 */
UCLASS()
class FPSCOMBATSYSTEM_API UFPSCombatAmmoWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	void HandleAmmoMessage(FGameplayTag Channel, const FFPSCombatAmmoChangedMessage& Message);

	UFUNCTION(BlueprintImplementableEvent)
	void OnAmmoUpdated(int32 CurrentAmmo, int32 ReserveAmmo);

	void RefreshFromCurrentWeapon();
	
	FGameplayMessageListenerHandle AmmoListenerHandle;
	
};
