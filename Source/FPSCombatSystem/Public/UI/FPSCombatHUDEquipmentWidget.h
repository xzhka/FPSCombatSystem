// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FPSCombatSystem/FPSCombatMessageTypes.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "FPSCombatHUDEquipmentWidget.generated.h"

/**
 * 
 */
UCLASS()
class FPSCOMBATSYSTEM_API UFPSCombatHUDEquipmentWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void SetVisibility(ESlateVisibility InVisibility) override;
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	UPROPERTY(EditDefaultsOnly, Category = "HUD")
	TSubclassOf<UFPSCombatEquipmentInstance> WatchedInstanceType;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "HUD")
	UFPSCombatEquipmentInstance* GetWatchedInstance() const;

	UPROPERTY(EditAnywhere, Category = "HUD")
	ESlateVisibility ShownVisibility = ESlateVisibility::Visible;
	
	UPROPERTY(EditAnywhere, Category = "HUD")
	ESlateVisibility HiddenVisibility = ESlateVisibility::Collapsed;
	
private:
	void RefreshWatchedInstance();
	void HandleEquipmentMessage(FGameplayTag Channel, const FFPSCombatEquipmentChangedMessage& Message);
	
	FGameplayMessageListenerHandle EquipmentListenerHandle;

	bool bWantsToVisible = true;
	
};
