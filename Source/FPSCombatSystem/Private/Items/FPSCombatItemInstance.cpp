// FPS Combat project


#include "Items/FPSCombatItemInstance.h"

#include "FPSCombatSystem/FPSCombatGameplayTags.h"
#include "FPSCombatSystem/FPSCombatMessageTypes.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Iris/ReplicationSystem/ReplicationFragmentUtil.h"
#include "Items/FPSCombatItemDefinition.h"
#include "Net/UnrealNetwork.h"

FString FFPSCombatTagInfo::GetDebugString() const
{
	return FString::Printf(TEXT("%sx%d"), *Tag.ToString(), Stack);
}

void FFPSCombatTagInfoContainer::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	for (int32 ItemIndex : RemovedIndices)
	{
		FFPSCombatTagInfo& Info = EntryList[ItemIndex];
		TagsToStack.Remove(Info.Tag);
		BroadcastChange(Info, Info.Stack, 0);
		Info.LastNoticedStack = 0;	
	}
}

void FFPSCombatTagInfoContainer::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for (int32 ItemIndex : AddedIndices)
	{
		FFPSCombatTagInfo& Info = EntryList[ItemIndex];
		TagsToStack.Add(Info.Tag, Info.Stack);
		BroadcastChange(Info, 0, Info.Stack);
		Info.LastNoticedStack = Info.Stack;	
	}
}

void FFPSCombatTagInfoContainer::PostReplicatedChange(const TArrayView<int32>& ChangedIndices, int32 FinalSize)
{
	for (int32 ItemIndex : ChangedIndices)
	{
		FFPSCombatTagInfo& Info = EntryList[ItemIndex];
		TagsToStack[Info.Tag] = Info.Stack;
		BroadcastChange(Info, Info.LastNoticedStack, Info.Stack);
		Info.LastNoticedStack = Info.Stack;
	}
}

void FFPSCombatTagInfoContainer::AddStack(FGameplayTag Tag, int32 StackCount)
{
	if (!Tag.IsValid())
	{
		return;
	}

	if (StackCount > 0)
	{
		for (FFPSCombatTagInfo& Entry : EntryList)
		{
			if (Entry.Tag == Tag)
			{
				const int32 OldCount = Entry.Stack;
				Entry.Stack += StackCount;
				TagsToStack[Tag] = Entry.Stack;
				MarkItemDirty(Entry);

				BroadcastChange(Entry, OldCount, Entry.Stack);
				Entry.LastNoticedStack = Entry.Stack;
				return;
			}
		}
		FFPSCombatTagInfo& NewEntry = EntryList.Emplace_GetRef(Tag, StackCount);
		MarkItemDirty(NewEntry);
		TagsToStack.Add(Tag, StackCount);

		BroadcastChange(NewEntry, 0, StackCount);
	}
}

void FFPSCombatTagInfoContainer::RemoveStack(FGameplayTag Tag, int32 StackCount)
{
	if (!Tag.IsValid())
	{
		return;
	}

	if (StackCount > 0)
	{
		for (auto It = EntryList.CreateIterator(); It; ++It)
		{
			FFPSCombatTagInfo& Stack = *It;
			if (Stack.Tag == Tag)
			{
				if (Stack.Stack <= StackCount)
				{
					BroadcastChange(Stack, Stack.Stack, 0);
					It.RemoveCurrent();
					TagsToStack.Remove(Tag);
					MarkArrayDirty();
				}
				else
				{
					const int32 OldCount = Stack.Stack;
					Stack.Stack -= StackCount;
					TagsToStack[Tag] = Stack.Stack;
					MarkItemDirty(Stack);

					BroadcastChange(Stack, OldCount, Stack.Stack);
					Stack.LastNoticedStack = Stack.Stack;
				}
				return;
			}
		}
	}
}

void FFPSCombatTagInfoContainer::BroadcastChange(const FFPSCombatTagInfo& Info, int32 OldCount, int32 NewCount)
{
	UObject* Outer = OwningOuter.Get();
	if (!Outer) return;
	
	UFPSCombatItemInstance* Instance = Cast<UFPSCombatItemInstance>(OwningOuter);
	AActor* OwnerActor = Instance ? Instance->GetTypedOuter<AActor>() : nullptr;
	if (!OwnerActor) return;

	FFPSCombatItemChangedMessage Message;
	Message.ItemInstance = Instance;
	Message.ItemDefinition = Instance->GetItemDefinition();
	Message.ItemTag = Info.Tag;
	Message.NewCount = NewCount;
	Message.DeltaCount = NewCount - OldCount;

	UGameplayMessageSubsystem::Get(OwnerActor).BroadcastMessage(FPSCombatGameplayTags::Message_Item_StackChange, Message);
}

int32 FFPSCombatTagInfoContainer::GetStack(FGameplayTag Tag) const
{
	const int32* StackCount = TagsToStack.Find(Tag);
	
	return StackCount ? *StackCount : 0;
}

UFPSCombatItemInstance::UFPSCombatItemInstance()
	: ItemStats(this)
{
}

void UFPSCombatItemInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	UObject::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UFPSCombatItemInstance, ItemDefinition);
	DOREPLIFETIME(UFPSCombatItemInstance, ItemStats);
}

bool UFPSCombatItemInstance::HasStatTag(FGameplayTag Tag) const
{
	return ItemStats.ContainsStack(Tag);
}

void UFPSCombatItemInstance::AddStackCount(FGameplayTag Tag, int32 StackCount)
{
	ItemStats.AddStack(Tag, StackCount);
}

void UFPSCombatItemInstance::RemoveStackCount(FGameplayTag Tag, int32 StackCount)
{
	ItemStats.RemoveStack(Tag, StackCount);
}

void UFPSCombatItemInstance::RegisterReplicationFragments(UE::Net::FFragmentRegistrationContext& Context,
                                                          UE::Net::EFragmentRegistrationFlags RegistrationFlags)
{
	using namespace UE::Net;

	FReplicationFragmentUtil::CreateAndRegisterFragmentsForObject(this, Context, RegistrationFlags);
}

int32 UFPSCombatItemInstance::GetDefaultStatsByValue(FGameplayTag Tag) const
{
	const UFPSCombatItemDefinition* DefCDO = GetDefault<UFPSCombatItemDefinition>(ItemDefinition);
	return DefCDO ? DefCDO->GetDefaultStatsValueByTag(Tag) : 0;
}

const UFPSCombatItemFragment* UFPSCombatItemInstance::FindFragmentByType(TSubclassOf<UFPSCombatItemFragment> FragmentType) const
{
	if ((FragmentType != nullptr) && (ItemDefinition != nullptr))
	{
		return GetDefault<UFPSCombatItemDefinition>(ItemDefinition)->FindFragmentByClass(FragmentType);
	}
	return nullptr;
}
