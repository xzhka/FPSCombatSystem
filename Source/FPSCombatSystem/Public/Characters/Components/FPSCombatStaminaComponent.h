// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FPSCombatBaseComponent.h"
#include "Components/ActorComponent.h"
#include "FPSCombatStaminaComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFPSCombatStaminaDeplatedEvent, AActor*, Investigator);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFPSCombatStaminaRestoredEvent, AActor*, Investigator);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FFPSCombatStaminaChangedEvent, UFPSCombatStaminaComponent*, StaminaComponent, float, OldValue, float, NewValue);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FPSCOMBATSYSTEM_API UFPSCombatStaminaComponent : public UFPSCombatBaseComponent
{
	GENERATED_BODY()

public:
	UFPSCombatStaminaComponent();

	UPROPERTY(BlueprintAssignable)
	FFPSCombatStaminaDeplatedEvent OnStaminaDepleted;

	UPROPERTY(BlueprintAssignable)
	FFPSCombatStaminaRestoredEvent OnStaminaRestored;

	UPROPERTY(BlueprintAssignable)
	FFPSCombatStaminaChangedEvent OnStaminaChanged;

	UPROPERTY(BlueprintAssignable)
	FFPSCombatStaminaChangedEvent OnMaxStaminaChanged;

	UFUNCTION(BlueprintCallable, Category="Components|Stamina")
	float GetStamina() const;

	UFUNCTION(BlueprintCallable, Category="Components|Stamina")
	float GetMaxStamina() const;

	UFUNCTION(BlueprintCallable, Category="Components|Stamina")
	float GetMergedStamina() const;

protected:
	virtual void BindAttributeDelegate() override;
	virtual void UnBindAttributeDelegate() override;
	virtual void HandleStaminaChanged(float OldValue, float NewValue);
	virtual void HandleMaxStaminaChanged(float OldValue, float NewValue);
	virtual void HandleStaminaDepletedChanged();
	virtual void HandleStaminaRestoredChanged();
	virtual float GetMerged() override { return GetMergedStamina(); };
};
