// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/FPSCombatAmmoWidget.h"

#include "Equipment/FPSCombatEquipmentManager.h"
#include "FPSCombatSystem/FPSCombatGameplayTags.h"
#include "Weapons/FPSCombatRangedWeaponInstance.h"

void UFPSCombatAmmoWidget::NativeConstruct()
{
	Super::NativeConstruct();
	AmmoListenerHandle = UGameplayMessageSubsystem::Get(this).RegisterListener(FPSCombatGameplayTags::Message_Ammo_Change,this, &UFPSCombatAmmoWidget::HandleAmmoMessage);
	
	RefreshFromCurrentWeapon();
}

void UFPSCombatAmmoWidget::NativeDestruct()
{
	AmmoListenerHandle.Unregister();
	
	Super::NativeDestruct();
}

void UFPSCombatAmmoWidget::HandleAmmoMessage(FGameplayTag Channel, const FFPSCombatAmmoChangedMessage& Message)
{
	OnAmmoUpdated(Message.CurrentAmmo, Message.ReserveAmmo);
}

void UFPSCombatAmmoWidget::RefreshFromCurrentWeapon()
{
	if (APawn* Pawn = GetOwningPlayerPawn())
	{
		if (UFPSCombatEquipmentManager* EqpManager = Pawn->FindComponentByClass<UFPSCombatEquipmentManager>())
		{
			if (UFPSCombatRangedWeaponInstance* RangedWeaponInstance = Cast<UFPSCombatRangedWeaponInstance>(EqpManager->GetFirstInstanceOfType(UFPSCombatRangedWeaponInstance::StaticClass())))
			{
				OnAmmoUpdated(RangedWeaponInstance->GetCurrentAmmo(), RangedWeaponInstance->GetReserveAmmo());
			}
		}
	}
}
