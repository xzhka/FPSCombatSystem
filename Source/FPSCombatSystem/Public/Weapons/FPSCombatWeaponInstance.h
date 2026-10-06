// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/FPSCombatAnimationTypes.h"
#include "Equipment/FPSCombatEquipmentInstance.h"
#include "FPSCombatWeaponInstance.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class FPSCOMBATSYSTEM_API UFPSCombatWeaponInstance : public UFPSCombatEquipmentInstance
{
	GENERATED_BODY()

	
public:
	virtual void OnEquipped() override;
	virtual void OnUnequipped() override;


	UFUNCTION(BlueprintCallable)
	void UpdateLastFireTime();

	UFUNCTION(BlueprintPure)
	float GetTimeFromLastInteraction() const;

	FORCEINLINE float GetTimeSinceLastFire() const { return GetWorld()->GetTimeSeconds()-TimeFired; }

	
protected:
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = Animations)
	TSubclassOf<UAnimInstance> PickAnimLayer(bool bIsEquipped) const;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animations)
	FFPSCombatLayerSelectionSet EquippedAnimSet;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animations)
	FFPSCombatLayerSelectionSet UnequippedAnimSet;
	
protected:
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = Animations)
	TSubclassOf<UAnimInstance> PickAnimLayer(bool bIsEquipped, const FGameplayTagContainer& CosmeticTag) const;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animations)
	FFPSCombatLayerSelectionSet EquippedAnimSet;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animations)
	FFPSCombatLayerSelectionSet UnequippedAnimSet;
	
private:

	float TimeEquipped = 0.f;
	float TimeFired = 0.f;
	
};
