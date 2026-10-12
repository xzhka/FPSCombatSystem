// FPS Combat project

#pragma once

#include "CoreMinimal.h"
#include "Engine/StreamableManager.h"
#include "Equipment/FPSCombatEquipmentInstance.h"
#include "Weapons/FPSCombatThrowableDefinition.h"
#include "FPSCombatThrowableInstance.generated.h"

/** UFPSCombatThrowableInstance
 * 
 *  A part of equipment instance which represents the throwable objects
 */
UCLASS(Blueprintable, BlueprintType)
class FPSCOMBATSYSTEM_API UFPSCombatThrowableInstance : public UFPSCombatEquipmentInstance
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	UFPSCombatThrowableDefinition* GetThrowableDefinition() const ;

	UFUNCTION(BlueprintCallable)
	bool HasChargesRemaining() const;

	UFUNCTION(BlueprintCallable)
	void ConsumeProjectile(FGameplayTag ProjectileTag) const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	TSubclassOf<AFPSCombatProjectileBase> GetProjectileClass() const { return CachedProjectileClass; }
		
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UAnimMontage* GetAnimDefinitionMontage(FGameplayTag ActionTag) const;
	
	virtual void OnEquipped() override;
	virtual void OnUnequipped() override;

private:
	void OnThrowAssetsLoaded();

	TSharedPtr<FStreamableHandle> ProjectileHandle; 
	
	UPROPERTY()
	TMap<FGameplayTag, TObjectPtr<UAnimMontage>> CachedActionMontages;

	UPROPERTY()
	TSubclassOf<AFPSCombatProjectileBase> CachedProjectileClass;
	
};
