// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FPSCombatItemInstance.h"
#include "Components/ActorComponent.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "FPSCombatItemManagerComponent.generated.h"

struct FFPSCombatItemList;
class UFPSCombatItemManagerComponent;

USTRUCT(BlueprintType)
struct FFPSCombatItemEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()
	FFPSCombatItemEntry() {}

	FString GetDebugString() const;

	
private:

	friend FFPSCombatItemList;
	friend UFPSCombatItemManagerComponent;

	UPROPERTY()
	TObjectPtr<UFPSCombatItemInstance> ItemInstance;
		
};

USTRUCT(BlueprintType)
struct FFPSCombatItemList : public FFastArraySerializer
{

	GENERATED_BODY()
	
	FFPSCombatItemList() {};
	
	FFPSCombatItemList(UActorComponent* ActorComp)
		: OwnerComponent(ActorComp)
	{}

public:
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	
	UFPSCombatItemInstance* AddEntry(TSubclassOf<UFPSCombatEquipmentDefinition> Definition, FGameplayTag Tag, int32 StackCount);
	UFPSCombatItemInstance* AddPreservedEntry(UFPSCombatItemInstance* InInstance);
	
	void RemoveEntry(UFPSCombatItemInstance* ItemInstance);
	
	void BroadcastEntryChange(UFPSCombatItemInstance* Instance, FGameplayTag Channel);

	
	UFPSCombatItemInstance* FindInstanceForDefinition(TSubclassOf<UFPSCombatEquipmentDefinition> Definition);
	
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FFPSCombatItemEntry, FFPSCombatItemList>(EntryList, DeltaParams, *this);
	}
	
private:
	
	friend UFPSCombatItemManagerComponent;
	
	UPROPERTY()
	TArray<FFPSCombatItemEntry> EntryList;
	
	UPROPERTY(NotReplicated)
	TObjectPtr<UActorComponent> OwnerComponent;
};

template<>
struct TStructOpsTypeTraits<FFPSCombatItemList> : public TStructOpsTypeTraitsBase2<FFPSCombatItemList>
{
	enum
	{ WithNetDeltaSerializer = true };
};




UCLASS()
class FPSCOMBATSYSTEM_API UFPSCombatItemManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UFPSCombatItemManagerComponent(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category= "Entry")
	UFPSCombatItemInstance* AddStack(TSubclassOf<UFPSCombatEquipmentDefinition> Definition, FGameplayTag Tag, int32 StackCount);

	UFUNCTION(BlueprintCallable, Category = "Entry")
	void AddInstance(UFPSCombatItemInstance* ItemInstance);
	
	UFUNCTION(BlueprintCallable, Category= "Entry")
	void RemoveInstance(UFPSCombatItemInstance* ItemInstance);

	UFUNCTION(BlueprintCallable, Category= "Entry")
	TArray<UFPSCombatItemInstance*> GetAllItemInstances() const;
	
	UFUNCTION(BlueprintCallable, Category= "Entry")
	UFPSCombatItemInstance* FindInstanceByDef(TSubclassOf<UFPSCombatEquipmentDefinition> Definition);
	
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void ReadyForReplication() override;

private:
	UPROPERTY(Replicated)
	FFPSCombatItemList ItemList;
};
