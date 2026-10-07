// FPS Combat project

#pragma once

#include "CoreMinimal.h"
#include "FPSCombatEquipmentDefinition.h"
#include "Items/FPSCombatItemInstance.h"
#include "FPSCombatEquipmentInstance.generated.h"

struct FFPSCombatEquipmentSpawnActor;
class APawn;


/** UFPSCombatEquipmentInstance
 * 
 *  A piece owned and applied to pawn
 */
UCLASS(Blueprintable, BlueprintType)
class FPSCOMBATSYSTEM_API UFPSCombatEquipmentInstance : public UObject
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void OnEquipped();
	virtual void OnUnequipped();
	
	virtual void SpawnEquipmentActors(const TArray<FFPSCombatEquipmentSpawnActor>& SpawnActors);
	virtual void ClearEquipmentActors();

	UFUNCTION(BlueprintCallable)
	void SetEquipmentActorsHidden(bool bHidden);

	UFUNCTION(BlueprintCallable)
	void SpawnEquipmentActorsFromInstance();
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	APawn* GetPawn() const;

	void SetDefinition(UFPSCombatEquipmentDefinition* InDefinition) { InstanceDefinition = InDefinition; }

	void SetItemInstance(UFPSCombatItemInstance* InIntemInstance) { Instance = InIntemInstance; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE UFPSCombatItemInstance* GetItemInstance() const { return Instance; }
	
	UFUNCTION(BlueprintPure, BlueprintCallable)
	FORCEINLINE UFPSCombatEquipmentDefinition* GetDefinition() const { return InstanceDefinition; }
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE TArray<AActor*> GetActorsToSpawn() const { return ActorsToSpawn; }

	virtual bool IsSupportedForNetworking() const override { return true; }

	virtual void RegisterReplicationFragments(UE::Net::FFragmentRegistrationContext& Context, UE::Net::EFragmentRegistrationFlags RegistrationFlags) override;

	UFUNCTION(BlueprintImplementableEvent, Category =Equipment)
	void K2_OnEquipped();

	UFUNCTION(BlueprintImplementableEvent, Category =Equipment)
	void K2_OnUnequipped();
	
private:

	UPROPERTY(Replicated)
	TObjectPtr<UFPSCombatItemInstance> Instance;
	
	UPROPERTY(Replicated)
	TObjectPtr<class UFPSCombatEquipmentDefinition> InstanceDefinition;
	
	UPROPERTY(Replicated)
	TArray<TObjectPtr<AActor>> ActorsToSpawn;
};
