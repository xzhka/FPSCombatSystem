// FPS Combat project

#pragma once

#include "CoreMinimal.h"
#include "FPSCombatEquipmentDefinition.h"
#include "FPSCombatEquipmentInstance.h"
#include "AbilitySystem/FPSCombatAbilitySet.h"
#include "Components/PawnComponent.h"
#include "Items/FPSCombatItemInstance.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "FPSCombatEquipmentManager.generated.h"

class UFPSCombatEquipmentManager;
struct FFPSCombatEquipmentList;

/* Single piece of applied equipment */
USTRUCT(BlueprintType)
struct FFPSCombatAppliedEquipmentEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()
	FFPSCombatAppliedEquipmentEntry() {}

	FString GetDebugString() const;

	
private:

	friend UFPSCombatEquipmentManager;
	friend FFPSCombatEquipmentList;
	
	UPROPERTY()
	TObjectPtr<UFPSCombatEquipmentInstance> Instance = nullptr; 

	UPROPERTY()
	TSubclassOf<UFPSCombatEquipmentDefinition> Definition;

	UPROPERTY(NotReplicated)
	FCombatAbilitySet_GrantedHandles GrantedHandles;
};

/* List of applied equipment */
USTRUCT(BlueprintType)
struct FFPSCombatEquipmentList : public FFastArraySerializer
{

	GENERATED_BODY()

	FFPSCombatEquipmentList() {}

public:

	
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	
	UFPSCombatEquipmentInstance* AddEntry(TSubclassOf<UFPSCombatEquipmentDefinition> EntryDefinition);
	void RemoveEntry(UFPSCombatEquipmentInstance* EquipmentInstance);

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FFPSCombatAppliedEquipmentEntry, FFPSCombatEquipmentList>(EntryList, DeltaParams, *this);
	}
	
private:

	UFPSCombatAbilitySystemComponent* GetASC() const;
	UFPSCombatEquipmentManager* GetEquipmentManager() const;
	
	friend UFPSCombatEquipmentManager;
	
	UPROPERTY()
	TArray<FFPSCombatAppliedEquipmentEntry> EntryList;
	
	UPROPERTY(NotReplicated)
	TObjectPtr<UActorComponent> OwnerComponent;
};

template<>
struct TStructOpsTypeTraits<FFPSCombatEquipmentList> : public TStructOpsTypeTraitsBase2<FFPSCombatEquipmentList>
{
	enum
	{ WithNetDeltaSerializer = true };
};


/*	UFPSCombatEquipmentManager
 *	
 *	Manages equipment applied to pawn
 */
UCLASS()
class FPSCOMBATSYSTEM_API UFPSCombatEquipmentManager : public UPawnComponent
{
	GENERATED_BODY()

	UFPSCombatEquipmentManager ( const FObjectInitializer& ObjectInitializer);

	
public:

	UFUNCTION(BlueprintCallable)
	UFPSCombatEquipmentInstance* OnEquipItem(TSubclassOf<UFPSCombatEquipmentDefinition> EquipDefinition, UFPSCombatItemInstance* ItemInstance);

	UFUNCTION(BlueprintCallable)
	void OnUnequipItem(UFPSCombatEquipmentInstance* ItemInstance);

	UFUNCTION(BlueprintCallable)
	void UnequipAll();
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void InitializeComponent() override;
	virtual void ReadyForReplication() override;
	virtual void UninitializeComponent() override;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UFPSCombatEquipmentInstance* GetFirstInstanceOfType(TSubclassOf<UFPSCombatEquipmentInstance> InstanceType);

	template <typename T>
	T* GetFirstInstanceOfType()
	{
		return (T*)GetFirstInstanceOfType(T::StaticClass());
	}

	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

	void BroadcastEquipmentChange(UFPSCombatEquipmentInstance* Instance, bool IsEquipped) const;
private:
	UPROPERTY(Replicated)
	FFPSCombatEquipmentList EquipmentList;
};
