// FPS Combat project

#pragma once

#include "CoreMinimal.h"
#include "ActorComponents/IKRigComponent.h"
#include "FPSCombatSystem/FPSCombatMessageTypes.h"
#include "Weapons/FPSCombatWeaponActor.h"
#include "FPSCombatRigIKComponent.generated.h"

/** UFPSCombatRigIKComponent
 *
 * IK component which treats the
 * bone sockets attach with weapons
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FPSCOMBATSYSTEM_API UFPSCombatRigIKComponent : public UIKRigComponent
{
	GENERATED_BODY()

	UFPSCombatRigIKComponent();

public:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;

protected:

	UFUNCTION()
	void OnEquipmentChange(FGameplayTag Channel,  const FFPSCombatEquipmentChangedMessage& Message);

	
	UPROPERTY(EditDefaultsOnly, Category = "Hand IK")
	FName LeftHandGoalName;

	UPROPERTY()
	TObjectPtr<AFPSCombatWeaponActor> CachedWeaponActor;
	
	FName CachedSocketName;
	
};
