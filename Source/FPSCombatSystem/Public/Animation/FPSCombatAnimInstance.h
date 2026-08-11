// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/Abilities/FPSCombatGameplayAbilitySprint.h"
#include "Animation/AnimInstance.h"
#include "FPSCombatAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class FPSCOMBATSYSTEM_API UFPSCombatAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UFPSCombatAnimInstance( const FObjectInitializer& ObjectInitializer);
	
	virtual void InitializeWithAbilitySystem(UAbilitySystemComponent* ASC);
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	
	UPROPERTY(BlueprintReadOnly, Category = "Locomotion", meta = (AllowPrivateAccess = "true"))
	bool bIsAirborne = false;

	UPROPERTY(BlueprintReadOnly, Category = "Ground Info")
	float GroundDistance = -1.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "GameplayTags")
	FGameplayTagBlueprintPropertyMap GameplayTagPropertyMap;
};
