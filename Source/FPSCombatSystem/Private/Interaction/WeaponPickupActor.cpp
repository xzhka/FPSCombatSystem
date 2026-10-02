// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/WeaponPickupActor.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemGlobals.h"
#include "Equipment/FPSCombatQuickBarComponent.h"
#include "FPSCombatSystem/FPSCombatGameplayTags.h"
#include "GameModes/FPSCombatPlayerState.h"
#include "Items/FPSCombatItemManagerComponent.h"

bool AWeaponPickupActor::TryGivePickup(APawn* PickupPawn)
{
	check(PickupPawn != nullptr);
	
	if (AController* Controller = PickupPawn->GetController())
	{
		UFPSCombatQuickBarComponent* QuickBar = Controller->GetComponentByClass<UFPSCombatQuickBarComponent>();

		if (AFPSCombatPlayerState* PlayerState = PickupPawn->GetPlayerState<AFPSCombatPlayerState>())
		{
			UFPSCombatItemManagerComponent* ItemManager = PlayerState->FindComponentByClass<UFPSCombatItemManagerComponent>();
		
			if (ItemManager != nullptr && QuickBar != nullptr)
			{
				UFPSCombatItemInstance* EquippedInstance = ItemManager->FindInstanceByDef(ItemDefinition);
				if (IsValid(EquippedInstance))
				{
					return TryTopUpStat(EquippedInstance);
				}
			
				const int32 FreeSlot = QuickBar->GetNextFreeItemSlot();
				if (FreeSlot == INDEX_NONE) return false;
				if (UFPSCombatItemInstance* ItemInstance = ItemManager->AddStack(ItemDefinition, FPSCombatGameplayTags::Item_Stat_Quantity, 1))
				{
					UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
						PickupPawn, FPSCombatGameplayTags::Ability_ExternalResolveRequested, FGameplayEventData());
					UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(PickupPawn);
					if (!ASC || ASC->HasMatchingGameplayTag(FPSCombatGameplayTags::Weapon_Reload)) return false;
					QuickBar->AddItemToSlot(ItemInstance, FreeSlot);
					
					QuickBar->SetActiveSlot(FreeSlot);
				
					return true;
				}
			}
		}
	}
	return false;	
}
