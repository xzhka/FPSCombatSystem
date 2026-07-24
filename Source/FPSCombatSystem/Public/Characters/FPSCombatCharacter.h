// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "FPSCombatCharacterPawnComp.h"
#include "AbilitySystem/FPSCombatAbilitySet.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "Components/FPSCombatHealthComponent.h"
#include "Components/FPSCombatStaminaComponent.h"
#include "Equipment/FPSCombatEquipmentManager.h"
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
	
	AFPSCombatCharacter();

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	
	// Get Camera Component
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FPSFollowCamera; }
	
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	virtual void UnPossessed() override;
	
	void InitializeAbilitySystem();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
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
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UFPSCombatAbilitySet> AbilitySet;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Definition")
	TSubclassOf<UFPSCombatWeaponDefinition> WeaponDefinition;
	
	FCombatAbilitySet_GrantedHandles GrantedHandles;
};
