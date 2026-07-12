// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/FPSCombatCharacter.h"
#include "Components/ActorComponent.h"
#include "FPSCombatMovementComp.generated.h"

struct FOnAttributeChangeData;

UENUM(BlueprintType)
enum class EFPSCombatMoveState : uint8
{
	Grounded = 0,
	Airborne
};




UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FPSCOMBATSYSTEM_API UFPSCombatMovementComp : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UFPSCombatMovementComp();

	UFUNCTION(BlueprintPure)
	static UFPSCombatMovementComp* GetMovementComp(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UFPSCombatMovementComp>() : nullptr); }

	UFUNCTION(BlueprintCallable, Category = "Movement State")
	EFPSCombatMoveState GetCurrentState() const { return CurrentMoveState; }
	
	virtual void BeginPlay() override;

	void InitializeWithAbilitySystem(UFPSCombatAbilitySystemComponent* ASC);

	virtual void Dash(float Strength, float Duration);
	
	virtual void Updraft(float Distance);


	
protected:
	void HandleMoveSpeedMultiplierChanged(const FOnAttributeChangeData& Data);
	
	UFUNCTION()
	void HandleMovementUpdated(float DeltaSeconds, FVector OldLocation, FVector OldVelocity);

	UFUNCTION()
	void HandleMovementModeChanged(ACharacter* Character, EMovementMode PrevMovementMode, uint8 PreviousCustomMode);


	UFUNCTION(BlueprintCallable, Category = "Movement State")
	void SetMovementState(EFPSCombatMoveState NewState);
	
	const float BaseWalkSpeed = 500.f;

	bool bIsWalking = false;

	bool bIsWalkingForward = false;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float Threshold = 10.f;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float ForwardDotThreshold = 0.3f;
	
	UPROPERTY()
	TObjectPtr<AFPSCombatCharacter> OwnerCharacter;

private:
	UPROPERTY()
	TObjectPtr<UFPSCombatAbilitySystemComponent> CachedASC;

	FVector GetDashDirection() const;
	
	UPROPERTY(VisibleAnywhere, Category = "Movement State")
	EFPSCombatMoveState CurrentMoveState = EFPSCombatMoveState::Grounded;
};
