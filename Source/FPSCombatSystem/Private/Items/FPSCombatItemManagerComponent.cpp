// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/FPSCombatItemManagerComponent.h"


#include "Engine/ActorChannel.h"
#include "FPSCombatSystem/FPSCombatGameplayTags.h"
#include "FPSCombatSystem/FPSCombatMessageTypes.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Net/UnrealNetwork.h"


FString FFPSCombatItemEntry::GetDebugString() const
{
	if (ItemInstance!=nullptr)
	{
		return FString::Printf(TEXT("%s"), *GetNameSafe(ItemInstance));
	}
	return FString();
}

void FFPSCombatItemList::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	for (int32 Index : RemovedIndices)
	{
		BroadcastEntryChange(EntryList[Index].ItemInstance, FPSCombatGameplayTags::Message_Item_Removed);
	}
}

void FFPSCombatItemList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for (int32 Index : AddedIndices)
	{
		BroadcastEntryChange(EntryList[Index].ItemInstance, FPSCombatGameplayTags::Message_Item_Added);
	}
}

UFPSCombatItemInstance* FFPSCombatItemList::AddEntry(TSubclassOf<UFPSCombatItemDefinition> Definition,
                                                     FGameplayTag Tag, int32 StackCount)
{
	check(OwnerComponent);
	
	AActor* Actor = OwnerComponent->GetOwner();
	check(Actor);
	check(Actor->HasAuthority());

	if (UFPSCombatItemInstance* Exist = FindInstanceForDefinition(Definition))
	{
		Exist->AddStackCount(Tag, StackCount);
		return Exist;
	}
	
	FFPSCombatItemEntry& NewEntry = EntryList.AddDefaulted_GetRef();
	NewEntry.ItemInstance = NewObject<UFPSCombatItemInstance>(OwnerComponent->GetOwner());
	NewEntry.ItemInstance->SetItemDefinition(Definition);
	NewEntry.ItemInstance->AddStackCount(Tag, StackCount);
	MarkItemDirty(NewEntry);
	BroadcastEntryChange(NewEntry.ItemInstance, FPSCombatGameplayTags::Message_Item_Added);
	
	return NewEntry.ItemInstance;
}

UFPSCombatItemInstance* FFPSCombatItemList::AddPreservedEntry(UFPSCombatItemInstance* InInstance)
{
	check(OwnerComponent);
	AActor* Actor = OwnerComponent->GetOwner();
	check(Actor);
	check(Actor->HasAuthority());

	if (!InInstance) return nullptr; 

	InInstance->Rename(nullptr, OwnerComponent->GetOwner());

	FFPSCombatItemEntry& NewEntry = EntryList.AddDefaulted_GetRef();
	NewEntry.ItemInstance = InInstance;

	MarkItemDirty(NewEntry);
	BroadcastEntryChange(NewEntry.ItemInstance, FPSCombatGameplayTags::Message_Item_Added);

	return NewEntry.ItemInstance;
}

void FFPSCombatItemList::RemoveEntry(UFPSCombatItemInstance* ItemInstance)
{
	check(OwnerComponent);
	AActor* Actor = OwnerComponent->GetOwner();
	check(Actor);
	check(Actor->HasAuthority());
	
	for (auto It = EntryList.CreateIterator(); It; ++It)
	{
		FFPSCombatItemEntry& Entry = *It;
		if (Entry.ItemInstance == ItemInstance)
		{
			BroadcastEntryChange(It->ItemInstance, FPSCombatGameplayTags::Message_Item_Removed);
			It.RemoveCurrent();

			MarkArrayDirty();
		}
	}
}

void FFPSCombatItemList::BroadcastEntryChange(UFPSCombatItemInstance* Instance, FGameplayTag Channel)
{
	if (!OwnerComponent || !Instance) return;

	FFPSCombatItemChangedMessage Message;
	
	Message.ItemInstance = Instance;
	Message.ItemDefinition = Instance ? Instance->GetItemDefinition() : nullptr;

	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(OwnerComponent->GetWorld());
	MessageSubsystem.BroadcastMessage(Channel, Message);
}

UFPSCombatItemInstance* FFPSCombatItemList::FindInstanceForDefinition(
	TSubclassOf<UFPSCombatItemDefinition> Definition)
{
	for (FFPSCombatItemEntry& Entry : EntryList)
	{
		if (Entry.ItemInstance && Entry.ItemInstance->GetItemDefinition() == Definition)
		{
			return Entry.ItemInstance;
		}
	}
	return nullptr;
}

UFPSCombatItemManagerComponent::UFPSCombatItemManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	  ItemList(this)
{
	SetIsReplicatedByDefault(true);
}

UFPSCombatItemInstance* UFPSCombatItemManagerComponent::AddStack(TSubclassOf<UFPSCombatItemDefinition> Definition,
	FGameplayTag Tag, int32 StackCount)
{
	UFPSCombatItemInstance* Result = nullptr;
	if (Definition!=nullptr)
	{
		Result = ItemList.AddEntry(Definition, Tag, StackCount);

		if (IsUsingRegisteredSubObjectList() && IsReadyForReplication() && Result)
		{
			AddReplicatedSubObject(Result);
		}
	}
	return Result;
}

void UFPSCombatItemManagerComponent::AddInstance(UFPSCombatItemInstance* ItemInstance)
{
	ItemList.AddPreservedEntry(ItemInstance);

	if (IsUsingRegisteredSubObjectList() && ItemInstance)
	{
		AddReplicatedSubObject(ItemInstance);
	}
}

void UFPSCombatItemManagerComponent::RemoveInstance(UFPSCombatItemInstance* ItemInstance)
{
	ItemList.RemoveEntry(ItemInstance);

	if (IsUsingRegisteredSubObjectList() && ItemInstance)
	{
		RemoveReplicatedSubObject(ItemInstance);
	}
}

TArray<UFPSCombatItemInstance*> UFPSCombatItemManagerComponent::GetAllItemInstances() const
{
	TArray<UFPSCombatItemInstance*> Result;
	Result.Reserve(ItemList.EntryList.Num());

	for(const FFPSCombatItemEntry& Entry : ItemList.EntryList)
	{
		if (Entry.ItemInstance)
		{
			Result.Add(Entry.ItemInstance);
		}
	}
	return Result;
}

UFPSCombatItemInstance* UFPSCombatItemManagerComponent::FindInstanceByDef(
	TSubclassOf<UFPSCombatItemDefinition> Definition)
{
	return ItemList.FindInstanceForDefinition(Definition);
}

bool UFPSCombatItemManagerComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch,
                                                         FReplicationFlags* RepFlags)
{
	bool ReplicateSuper = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for (FFPSCombatItemEntry& Entry : ItemList.EntryList)
	{
		UFPSCombatItemInstance* Instance = Entry.ItemInstance;
		if (Instance && IsValid(Instance))
		{
			ReplicateSuper |= Channel->ReplicateSubobject(Instance, *Bunch, *RepFlags);
		}
	}
	return ReplicateSuper;
}

void UFPSCombatItemManagerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UFPSCombatItemManagerComponent, ItemList);
}

void UFPSCombatItemManagerComponent::ReadyForReplication()
{
	Super::ReadyForReplication();

	if (IsUsingRegisteredSubObjectList())
	{
		for (const FFPSCombatItemEntry& Entry : ItemList.EntryList)
		{
			UFPSCombatItemInstance* Instance = Entry.ItemInstance;

			if (Instance && IsValid(Instance))
			{
				AddReplicatedSubObject(Instance);
			}
		}
	}
}

