// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FPSCombatInfoWidget.h"
#include "Blueprint/UserWidget.h"
#include "FPSCombatSystem/FPSCombatMessageTypes.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "FPSCombatHUDEquipmentWidget.generated.h"

/**
 * 
 */
UCLASS()
class FPSCOMBATSYSTEM_API UFPSCombatHUDEquipmentWidget : public UFPSCombatInfoWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	UPROPERTY(EditDefaultsOnly, Category = "HUD")
	TSubclassOf<UFPSCombatEquipmentInstance> WatchedInstanceType;
	
private:
	void RefreshWatchedInstance();
	void HandleEquipmentMessage(FGameplayTag Channel, const FFPSCombatEquipmentChangedMessage& Message);
	
	FGameplayMessageListenerHandle EquipmentListenerHandle;

	bool bWantsToVisible = true;
	
};
