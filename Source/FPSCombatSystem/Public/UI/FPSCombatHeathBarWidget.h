// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Characters/Components/FPSCombatHealthComponent.h"
#include "FPSCombatHeathBarWidget.generated.h"

/**
 * 
 */
UCLASS()
class FPSCOMBATSYSTEM_API UFPSCombatHeathBarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	
	void InitializeWithHealthComponent(UFPSCombatHealthComponent* HC);
	
protected:

	UPROPERTY(EditAnywhere, Category = "Health")
	TWeakObjectPtr<UFPSCombatHealthComponent> HealthComponent;
	
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnHealthUpdated(float Percent);

	UFUNCTION()
	void HandleHealthUpdated(AActor* Instigator, float OldValue, float NewValue, UFPSCombatHealthComponent* HC);
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION()
	void HandlePawnChanged(APawn* OldPawn, APawn* NewPawn);
	
};
