// FPS Combat project

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/FPSCombatGameplayAbilitySprint.h"
#include "Animation/AnimInstance.h"
#include "FPSCombatAnimInstance.generated.h"

/** UFPSCombatAnimInstance
 *
 * Animation instance represented
 * pawn animation class
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

	UPROPERTY(BlueprintReadOnly, Category = "Ground Info")
	float GroundDistance = -1.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "GameplayTags")
	FGameplayTagBlueprintPropertyMap GameplayTagPropertyMap;
};
