// FPS Combat project

#pragma once

#include "CoreMinimal.h"
#include "Characters/FPSCombatCharacter.h"
#include "Components/ActorComponent.h"
#include "FPSCombatMovementComp.generated.h"

struct FOnAttributeChangeData;

/* Enum store state of pawn */
UENUM(BlueprintType)
enum class EFPSCombatMoveState : uint8
{
	Grounded = 0,
	Airborne
};

/*	FPSCombatGroundInfo
 *	
 *	Information about the ground under the pawn
 */
USTRUCT(BlueprintType)
struct FPSCombatGroundInfo
{
	GENERATED_BODY()

	FPSCombatGroundInfo()
	  :	LastUpdateFrame(0),
		GroundDistance(0.f)
	{}


	uint64 LastUpdateFrame;

	UPROPERTY(BlueprintReadOnly)
	float GroundDistance;

	
	UPROPERTY(BlueprintReadOnly)
	FHitResult GroundInfoHitResult;
	
};

/*	UFPSCombatMovementComp
 *	
 *	Actor component representing
 *	pawn state, movement abilties
 *  and move speed affectors 
 */
UCLASS( Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FPSCOMBATSYSTEM_API UFPSCombatMovementComp : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UFPSCombatMovementComp();

	UFUNCTION(BlueprintPure)
	static UFPSCombatMovementComp* GetMovementComp(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UFPSCombatMovementComp>() : nullptr); }

	UFUNCTION(BlueprintCallable, Category = "Movement State", meta = (BlueprintThreadSafe))
	FORCEINLINE EFPSCombatMoveState GetCurrentState() const { return CurrentMoveState; }

	UFUNCTION(BlueprintCallable, Category = "Groung Info")
	const FPSCombatGroundInfo& GetGroundInfo();

	UFUNCTION(BlueprintPure, Category= "Speed")
	float GetMoveSpeedMultiplier() const;
	
	virtual void BeginPlay() override;

	void InitializeWithAbilitySystem(UFPSCombatAbilitySystemComponent* ASC);

	void UninitializeFromAbilitySystem();
	
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

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float Threshold = 3.f;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float ForwardDotThreshold = 0.3f;
	
	UPROPERTY()
	TObjectPtr<AFPSCombatCharacter> OwnerCharacter;

private:
	void ClearASCGameplayTags();
	
	UPROPERTY()
	TObjectPtr<UFPSCombatAbilitySystemComponent> CachedASC;

	FPSCombatGroundInfo CachedGroundInfo;
	
	FVector GetDashDirection() const;
	
	UPROPERTY(VisibleAnywhere, Category = "Movement State")
	EFPSCombatMoveState CurrentMoveState = EFPSCombatMoveState::Grounded;
};
