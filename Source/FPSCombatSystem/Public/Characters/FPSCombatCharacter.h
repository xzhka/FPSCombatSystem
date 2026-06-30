// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "Components/FPSCombatHealthComponent.h"
#include "FPSCombatCharacter.generated.h"


UCLASS()
class FPSCOMBATSYSTEM_API AFPSCombatCharacter : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FPSFollowCamera;
	
public:
	
	AFPSCombatCharacter();


	
	
	// Get Camera Component
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FPSFollowCamera; }
	virtual void PossessedBy(AController* NewController) override;

	void InitializeAbilitySystem();

	
protected:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health")
	TObjectPtr<UFPSCombatHealthComponent> HealthComponent;
	
};
