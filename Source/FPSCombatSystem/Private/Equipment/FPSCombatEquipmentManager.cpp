// FPS Combat project


#include "Equipment/FPSCombatEquipmentManager.h"
#include "AbilitySystemGlobals.h"
#include "Engine/ActorChannel.h"
#include "FPSCombatSystem/FPSCombatGameplayTags.h"
#include "FPSCombatSystem/FPSCombatMessageTypes.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Net/UnrealNetwork.h"

FString FFPSCombatAppliedEquipmentEntry::GetDebugString() const
{
	return FString::Printf(TEXT("Insance: %s. Definition: %s"), *GetNameSafe(Instance), *GetNameSafe(Definition.Get()));
}

void FFPSCombatEquipmentList::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	for (int32 Index : RemovedIndices)
	{
		FFPSCombatAppliedEquipmentEntry& Entry = EntryList[Index];
		if (Entry.Instance != nullptr)
		{
			Entry.Instance->OnUnequipped();
			if (UFPSCombatEquipmentManager* Manager = GetEquipmentManager())
			{
				Manager->BroadcastEquipmentChange(Entry.Instance, false);
			}
		}
	}
}

void FFPSCombatEquipmentList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for (int32 Index : AddedIndices)
	{
		FFPSCombatAppliedEquipmentEntry& Entry = EntryList[Index];
		if (Entry.Instance != nullptr)
		{
			Entry.Instance->SetDefinition(GetMutableDefault<UFPSCombatEquipmentDefinition>(Entry.Definition));
			Entry.Instance->OnEquipped();
			if (UFPSCombatEquipmentManager* Manager = GetEquipmentManager())
			{
				Manager->BroadcastEquipmentChange(Entry.Instance, true);
			}
		}
	}
}

UFPSCombatEquipmentInstance* FFPSCombatEquipmentList::AddEntry(TSubclassOf<UFPSCombatEquipmentDefinition> EntryDefinition)
{
	UFPSCombatEquipmentInstance* ResultInstance = nullptr;

	check(EntryDefinition != nullptr);
	check(OwnerComponent);
	check(OwnerComponent->GetOwner()->HasAuthority());

	
	const UFPSCombatEquipmentDefinition* DefinitionCDO = GetDefault<UFPSCombatEquipmentDefinition>(EntryDefinition);
	
	
	TSubclassOf<UFPSCombatEquipmentInstance> InstanceType = DefinitionCDO->ActorEquipmentClass;
	if (InstanceType == nullptr)
	{
		InstanceType = UFPSCombatEquipmentInstance::StaticClass();
	}

	FFPSCombatAppliedEquipmentEntry& NewEntry = EntryList.AddDefaulted_GetRef();
	NewEntry.Definition = EntryDefinition;

	if (AActor* Actor = OwnerComponent->GetOwner())
	{
		NewEntry.Instance = NewObject<UFPSCombatEquipmentInstance>(Actor, InstanceType);	
	}
	ResultInstance = NewEntry.Instance;

	if (ResultInstance)
	{
		ResultInstance->SetDefinition(GetMutableDefault<UFPSCombatEquipmentDefinition>(EntryDefinition));
	}
	
	if (UFPSCombatAbilitySystemComponent* ASC = GetASC())
	{
		for (const TObjectPtr<const UFPSCombatAbilitySet>& AbilitySet : DefinitionCDO->AbilitySet)
		{
			AbilitySet->GiveAbility(ASC, &NewEntry.GrantedHandles, ResultInstance);
		}
	}
	ResultInstance->SpawnEquipmentActors(DefinitionCDO->SpawnActors);

	MarkItemDirty(NewEntry);
	
	return ResultInstance;
}

void FFPSCombatEquipmentList::RemoveEntry(UFPSCombatEquipmentInstance* EquipmentInstance)
{
	for (auto Iterator = EntryList.CreateIterator(); Iterator; ++Iterator)
	{
		FFPSCombatAppliedEquipmentEntry& Entry = *Iterator;
		if (Entry.Instance == EquipmentInstance)
		{
			if (UFPSCombatAbilitySystemComponent* ASC = GetASC())
			{
				Entry.GrantedHandles.ClearAbilitySystem(ASC);
			}
			EquipmentInstance->ClearEquipmentActors();

			Iterator.RemoveCurrent();
			
			MarkArrayDirty();
		}
	}
}

UFPSCombatAbilitySystemComponent* FFPSCombatEquipmentList::GetASC() const
{
	check(OwnerComponent);
	const AActor* OwningActor = OwnerComponent->GetOwner();

	return Cast<UFPSCombatAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwningActor));
}

UFPSCombatEquipmentManager* FFPSCombatEquipmentList::GetEquipmentManager() const
{
	check(OwnerComponent);

	return Cast<UFPSCombatEquipmentManager>(OwnerComponent);
}

UFPSCombatEquipmentManager::UFPSCombatEquipmentManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);
	
	bWantsInitializeComponent = true;
	EquipmentList.OwnerComponent = this;
}

UFPSCombatEquipmentInstance* UFPSCombatEquipmentManager::OnEquipItem(
	TSubclassOf<UFPSCombatEquipmentDefinition> EquipDefinition, UFPSCombatItemInstance* ItemInstance)
{
	UFPSCombatEquipmentInstance* Result = nullptr;
	if (EquipDefinition != nullptr)
	{
		Result = EquipmentList.AddEntry(EquipDefinition);
		if (Result != nullptr)
		{
			Result->SetItemInstance(ItemInstance);
			Result->OnEquipped();
			BroadcastEquipmentChange(Result, true);
			
			if (IsUsingRegisteredSubObjectList() && IsReadyForReplication())
			{
				AddReplicatedSubObject(Result);
			}
		}
	}
	return Result;
}

void UFPSCombatEquipmentManager::OnUnequipItem(UFPSCombatEquipmentInstance* ItemInstance)
{
	if (ItemInstance != nullptr)
	{
		if (IsUsingRegisteredSubObjectList())
		{
			RemoveReplicatedSubObject(ItemInstance);
		}

		ItemInstance->OnUnequipped();
		BroadcastEquipmentChange(ItemInstance, false);
		EquipmentList.RemoveEntry(ItemInstance);
	}
}

void UFPSCombatEquipmentManager::UnequipAll()
{
	TArray<UFPSCombatEquipmentInstance*> AllInstances;

	for (const FFPSCombatAppliedEquipmentEntry& Entry : EquipmentList.EntryList)
	{
		AllInstances.Add(Entry.Instance);
	}

	for (UFPSCombatEquipmentInstance* UnequipInstance : AllInstances)
	{
		OnUnequipItem(UnequipInstance);
	}
}

void UFPSCombatEquipmentManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UFPSCombatEquipmentManager, EquipmentList);
	
}

void UFPSCombatEquipmentManager::InitializeComponent()
{
	Super::InitializeComponent();
}

void UFPSCombatEquipmentManager::ReadyForReplication()
{
	Super::ReadyForReplication();
	if (IsUsingRegisteredSubObjectList())
	{
		for (const FFPSCombatAppliedEquipmentEntry& Entry : EquipmentList.EntryList)
		{
			UFPSCombatEquipmentInstance* Instance = Entry.Instance;
			if (IsValid(Instance))
			{
				AddReplicatedSubObject(Instance);
			}
		}
	}
	
}

void UFPSCombatEquipmentManager::UninitializeComponent()
{
	UnequipAll();
	
	Super::UninitializeComponent();
}

UFPSCombatEquipmentInstance* UFPSCombatEquipmentManager::GetFirstInstanceOfType(
	TSubclassOf<UFPSCombatEquipmentInstance> InstanceType)
{
	if (InstanceType != nullptr)
	{
		for (FFPSCombatAppliedEquipmentEntry& Entry : EquipmentList.EntryList)
		{
			if (Entry.Instance && Entry.Instance->IsA(InstanceType))
			{
				return Entry.Instance;
			}
		}
	}
	return nullptr;
}

bool UFPSCombatEquipmentManager::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch,
	FReplicationFlags* RepFlags)
{
	bool ReplicateSuper = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for (FFPSCombatAppliedEquipmentEntry& Entry : EquipmentList.EntryList)
	{
		UFPSCombatEquipmentInstance* Instance = Entry.Instance;
		if (IsValid(Instance))
		{
			ReplicateSuper |= Channel->ReplicateSubobject(Instance, *Bunch, *RepFlags);
		}
	}
	return ReplicateSuper;
}

void UFPSCombatEquipmentManager::BroadcastEquipmentChange(UFPSCombatEquipmentInstance* Instance, bool IsEquipped) const
{
	FFPSCombatEquipmentChangedMessage EquipmentMessage;

	EquipmentMessage.NewObjectInstance = Instance;
	EquipmentMessage.bIsEquipped = IsEquipped;

	UGameplayMessageSubsystem::Get(this).BroadcastMessage(FPSCombatGameplayTags::Message_Equipment_Change, EquipmentMessage);
}
