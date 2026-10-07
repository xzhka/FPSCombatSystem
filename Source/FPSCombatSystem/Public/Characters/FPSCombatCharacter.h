// FPS Combat project

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "FPSCombatCharacterPawnComp.h"
#include "Animation/FPSCombatRigIKComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/FPSCombatHealthComponent.h"
#include "Components/FPSCombatStaminaComponent.h"
#include "Equipment/FPSCombatEquipmentManager.h"
#include "GameFramework/Character.h"
#include "Items/FPSCombatItemManagerComponent.h"
#include "Weapons/FPSCombatThrowableDefinition.h"
#include "FPSCombatCharacter.generated.h"

class UFPSCombatMovementComp;


/*	AFPSCombatCharacter
 *	
 *	Main base character pawn class
 */
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
	
	
	/* Get Camera Component */
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FPSFollowCamera; }

	UFUNCTION(BlueprintPure, Category = "Movement" , meta = (BlueprintThreadSafe))
	FORCEINLINE UFPSCombatMovementComp* GetCombatMovementComponent() const { return MovementComponent; }

	/* ACharacter override functions*/
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	virtual void UnPossessed() override;
	virtual void OnConstruction(const FTransform& Transform) override;

	/* Ability system functions */
	void InitializeAbilitySystem();
	void UninitializeAbilitySystem();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	/* Death implementation functions*/
	
	UFUNCTION()
	void OnDeathStarted(AActor* OwningActor);
	
	UFUNCTION()
	void OnDeathEnded(AActor* OwningActor);

	UFUNCTION(BlueprintImplementableEvent)
	void K2_OnDeathEnds();

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="On Death Started"))
	void K2_OnDeathStarts(const FFPSCombatDeathInfo& DeathInfo);
	
	void ClearActorDueDeath();
	
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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	TObjectPtr<UFPSCombatRigIKComponent> RigIKComponent;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	TSubclassOf<UAnimInstance> DefaultAnimClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Definition")
	TSubclassOf<UFPSCombatThrowableDefinition> ThrowableDefinition;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Definition")
	TSubclassOf<UFPSCombatItemDefinition> ThrowableItemDefinition;

private:
	UPROPERTY()
	TWeakObjectPtr<UFPSCombatAbilitySystemComponent> CachedASC;
	
	void GrantDefaultEquipment();
};
