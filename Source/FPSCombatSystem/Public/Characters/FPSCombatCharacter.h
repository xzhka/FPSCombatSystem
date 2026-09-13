// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "FPSCombatCharacterPawnComp.h"
#include "Camera/CameraComponent.h"
#include "Components/FPSCombatHealthComponent.h"
#include "Components/FPSCombatStaminaComponent.h"
#include "Equipment/FPSCombatEquipmentManager.h"
#include "GameFramework/Character.h"
#include "Items/FPSCombatItemManagerComponent.h"
#include "Weapons/FPSCombatThrowableDefinition.h"
#include "Weapons/FPSCombatWeaponDefinition.h"
#include "FPSCombatCharacter.generated.h"

class UFPSCombatMovementComp;

UCLASS()
class FPSCOMBATSYSTEM_API AFPSCombatCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FPSFollowCamera;
	
public:

	/* Functions */
	
	AFPSCombatCharacter();
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	
	// Get Camera Component
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FPSFollowCamera; }

	UFUNCTION(BlueprintPure, Category = "Movement" , meta = (BlueprintThreadSafe))
	FORCEINLINE UFPSCombatMovementComp* GetCombatMovementComponent() const { return MovementComponent; }
	
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	virtual void UnPossessed() override;
	
	void InitializeAbilitySystem();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual void OnConstruction(const FTransform& Transform) override;

protected:
	void GrantDefaultEquipment();
	
	/*Components initialize*/
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health")
	TObjectPtr<UFPSCombatHealthComponent> HealthComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PawnComponent")
	TObjectPtr<UFPSCombatCharacterPawnComp> PawnComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stamina")
	TObjectPtr<UFPSCombatStaminaComponent> StaminaComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	TObjectPtr<UFPSCombatMovementComp> MovementComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment")
	TObjectPtr<UFPSCombatEquipmentManager> EquipmentComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Items")
	TObjectPtr<UFPSCombatItemManagerComponent> ItemComponent;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	TSubclassOf<UAnimInstance> DefaultAnimClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Definition")
	TSubclassOf<UFPSCombatThrowableDefinition> ThrowableDefinition;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Definition")
	TSubclassOf<UFPSCombatItemDefinition> ThrowableItemDefinition;
};
