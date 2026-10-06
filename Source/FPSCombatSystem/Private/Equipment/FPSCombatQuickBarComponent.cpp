// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipment/FPSCombatQuickBarComponent.h"

#include "FPSCombatSystem/FPSCombatMessageTypes.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Items/FPSCombatItemDefinition.h"
#include "Net/UnrealNetwork.h"

UE_DEFINE_GAMEPLAY_TAG_STATIC(Message_Slot_Changed, "Message.Slot.Changed");
UE_DEFINE_GAMEPLAY_TAG_STATIC(Message_ActiveSlot_Changed, "Message.ActiveSlot.Changed");

UFPSCombatQuickBarComponent::UFPSCombatQuickBarComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);
}

void UFPSCombatQuickBarComponent::BeginPlay()
{
	if (ItemSlots.Num() < SlotsAmount)
	{
		ItemSlots.AddDefaulted(SlotsAmount - ItemSlots.Num());
	}
	
	
	Super::BeginPlay();
}

void UFPSCombatQuickBarComponent::CycleSlotForward()
{
	if (ItemSlots.Num() < 2)
	{
		return;
	}

	const int32 OldIndex = ActiveSlot < 0 ? ItemSlots.Num()-1 : ActiveSlot;
	int32 NewIndex = ActiveSlot;
	do
	{
		NewIndex = (NewIndex + 1) % ItemSlots.Num();
		if (ItemSlots[NewIndex] != nullptr)
		{
			SetActiveSlot(NewIndex);
			return;
		}
	}
	while (NewIndex != OldIndex);
}

void UFPSCombatQuickBarComponent::CycleSlotBackward()
{
	if (ItemSlots.Num() < 2)
	{
		return;
	}

	const int32 OldIndex = ActiveSlot < 0 ? ItemSlots.Num()-1 : ActiveSlot;
	int32 NewIndex = ActiveSlot;
	do
	{
		NewIndex = (NewIndex - 1 + ItemSlots.Num()) % ItemSlots.Num();
		if (ItemSlots[NewIndex] != nullptr)
		{
			SetActiveSlot(NewIndex);
			return;
		}
	}
	while (NewIndex != OldIndex);
}

void UFPSCombatQuickBarComponent::EquipItemInSlot()
{
	check(EquippedItem == nullptr);

	if (ItemSlots.IsValidIndex(ActiveSlot))
	{
		if (UFPSCombatItemInstance* Instance = ItemSlots[ActiveSlot])
		{
			if (const UFPSCombatItemFragment_Equippable* EquippableFragment = Instance->FindFragmentByType<UFPSCombatItemFragment_Equippable>())
			{
				TSubclassOf<UFPSCombatEquipmentDefinition> EquipDefinition = EquippableFragment->EquipmentDefinition;
				if (EquipDefinition != nullptr)
				{
					if (UFPSCombatEquipmentManager* EquipmentManager = FindEquipmentManager())
					{
						EquippedItem = EquipmentManager->OnEquipItem(EquipDefinition, Instance);
					}
				}
			}
		}
	}
}

void UFPSCombatQuickBarComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UFPSCombatQuickBarComponent, ActiveSlot);
	DOREPLIFETIME(UFPSCombatQuickBarComponent, ItemSlots);
}

void UFPSCombatQuickBarComponent::SetActiveSlot_Implementation(int32 NewIndex)
{
	if ((ItemSlots.IsValidIndex(NewIndex)) && (ActiveSlot != NewIndex))
	{
		UnequipItemInSlot();

		ActiveSlot = NewIndex;
		
		EquipItemInSlot();

		OnRep_ActiveSlot();
	}
}

UFPSCombatItemInstance* UFPSCombatQuickBarComponent::GetActiveSlotItem() const
{
	return ItemSlots.IsValidIndex(ActiveSlot) ? ItemSlots[ActiveSlot] : nullptr;
}

void UFPSCombatQuickBarComponent::AddItemToSlot(UFPSCombatItemInstance* Item, int32 ItemSlot)
{
	if (ItemSlots.IsValidIndex(ItemSlot) && Item)
	{
		if (ItemSlots[ItemSlot] == nullptr)
		{
			ItemSlots[ItemSlot] = Item;
			OnRep_ItemSlots();
		}
	}
}

UFPSCombatItemInstance* UFPSCombatQuickBarComponent::RemoveItemFromSlot(int32 ItemSlot)
{
	UFPSCombatItemInstance* Result = nullptr;

	if (ActiveSlot == ItemSlot)
	{
		UnequipItemInSlot();
		ActiveSlot = -1;
	}

	if (ItemSlots.IsValidIndex(ItemSlot))
	{
		Result = ItemSlots[ItemSlot];

		if (ItemSlots[ItemSlot] != nullptr)
		{
			ItemSlots[ItemSlot] = nullptr;
			OnRep_ItemSlots();
		}
	}
	return Result;
}

int32 UFPSCombatQuickBarComponent::GetNextFreeItemSlot()
{
	int32 FreeItemSlot = 0;
	for (const TObjectPtr<UFPSCombatItemInstance>& Instance : ItemSlots)
	{
		if (Instance == nullptr)
		{
			return FreeItemSlot;
		}
		++FreeItemSlot;
	}
	return INDEX_NONE;
}

void UFPSCombatQuickBarComponent::UnequipItemInSlot()
{
	if (UFPSCombatEquipmentManager* Manager = FindEquipmentManager())
	{
		if (EquippedItem!= nullptr)
		{
			Manager->OnUnequipItem(EquippedItem);
			EquippedItem = nullptr;
		}
	}
}

void UFPSCombatQuickBarComponent::OnRep_ItemSlots()
{
	FFPSCombatSlotsChangedMessage Message;

	Message.OwnerActor = GetOwner();
	Message.ItemSlots = ItemSlots;

	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
	MessageSubsystem.BroadcastMessage(Message_Slot_Changed, Message);
}

void UFPSCombatQuickBarComponent::OnRep_ActiveSlot()
{
	FFPSCombatActiveSlotChangedMessage Message;

	Message.OwnerActor = GetOwner();
	Message.NewActiveSlot = ActiveSlot;

	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
	MessageSubsystem.BroadcastMessage(Message_ActiveSlot_Changed, Message);
}

UFPSCombatEquipmentManager* UFPSCombatQuickBarComponent::FindEquipmentManager() const
{	
	if (AController* Controller = Cast<AController>(GetOwner()))
	{
		if (APawn* Pawn = Controller->GetPawn())
		{
			return Pawn->FindComponentByClass<UFPSCombatEquipmentManager>();
		}
	}
	return nullptr;
}
