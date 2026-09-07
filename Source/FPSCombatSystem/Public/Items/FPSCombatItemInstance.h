// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Equipment/FPSCombatEquipmentDefinition.h"
#include "FPSCombatItemInstance.generated.h"

struct FFPSCombatTagInfoContainer;
class UFPSCombatItemInstance;

USTRUCT(BlueprintType)
struct FFPSCombatTagInfo : public FFastArraySerializerItem
{
	GENERATED_BODY()
	FFPSCombatTagInfo() {}

	FFPSCombatTagInfo(FGameplayTag InTag, int32 InStack)
		: Tag(InTag), Stack(InStack)
	{}
	
	
	FString GetDebugString() const;

	
private:

	friend FFPSCombatTagInfoContainer;

	UPROPERTY()
	FGameplayTag Tag;

	UPROPERTY()
	int32 Stack = 0;

	UPROPERTY(NotReplicated)
	int32 LastNoticedStack = INDEX_NONE;
};

USTRUCT(BlueprintType)
struct FFPSCombatTagInfoContainer : public FFastArraySerializer
{

	GENERATED_BODY()
	
	FFPSCombatTagInfoContainer() {}

	FFPSCombatTagInfoContainer(UObject* InOwningInstance)
		: OwningOuter(InOwningInstance)
	{}

public:

	
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange( const TArrayView< int32 >& ChangedIndices, int32 FinalSize);
	
	void AddStack(FGameplayTag Tag, int32 StackCount);
	void RemoveStack(FGameplayTag Tag, int32 StackCount);

	void BroadcastChange(const FFPSCombatTagInfo& Info, int32 OldCount, int32 NewCount);
	
	int32 GetStack(FGameplayTag Tag) const; 

	bool ContainsStack(FGameplayTag Tag) const { return TagsToStack.Contains(Tag); }
	
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FFPSCombatTagInfo, FFPSCombatTagInfoContainer>(EntryList, DeltaParams, *this);
	}
	
private:
	UPROPERTY()
	TArray<FFPSCombatTagInfo> EntryList;
	
	TMap<FGameplayTag, int32> TagsToStack;

	UPROPERTY(NotReplicated)
	TWeakObjectPtr<UObject> OwningOuter;
};

template<>
struct TStructOpsTypeTraits<FFPSCombatTagInfoContainer> : public TStructOpsTypeTraitsBase2<FFPSCombatTagInfoContainer>
{
	enum
	{ WithNetDeltaSerializer = true };
};


UCLASS(Blueprintable, BlueprintType)
class FPSCOMBATSYSTEM_API UFPSCombatItemInstance : public UObject
{
	GENERATED_BODY()

public:
	UFPSCombatItemInstance();
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	TSubclassOf<UFPSCombatEquipmentDefinition> GetItemDefinition() const { return ItemDefinition; }
	void SetItemDefinition(const TSubclassOf<UFPSCombatEquipmentDefinition> InItemDefinition) { ItemDefinition = InItemDefinition; };
	
	bool HasStatTag(FGameplayTag Tag) const;
	int32 GetStack(FGameplayTag Tag) const { return ItemStats.GetStack(Tag); };

	void AddStackCount(FGameplayTag Tag, int32 StackCount);
	void RemoveStackCount(FGameplayTag Tag, int32 StackCount);
	
	virtual void RegisterReplicationFragments(UE::Net::FFragmentRegistrationContext& Context,
		UE::Net::EFragmentRegistrationFlags RegistrationFlags) override;
	virtual bool IsSupportedForNetworking() const override { return true; }

private:
	
	UPROPERTY(Replicated)
	TSubclassOf<UFPSCombatEquipmentDefinition> ItemDefinition;

	UPROPERTY(Replicated)
	FFPSCombatTagInfoContainer ItemStats;
};
