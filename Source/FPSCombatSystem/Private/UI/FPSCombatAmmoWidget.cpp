// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/FPSCombatAmmoWidget.h"

#include "Equipment/FPSCombatEquipmentManager.h"
#include "FPSCombatSystem/FPSCombatGameplayTags.h"
#include "Weapons/FPSCombatRangedWeaponInstance.h"

void UFPSCombatAmmoWidget::NativeConstruct()
{
	Super::NativeConstruct();
	EquipmentListenerHandle = UGameplayMessageSubsystem::Get(this).RegisterListener(FPSCombatGameplayTags::Message_Equipment_Change, this, &UFPSCombatAmmoWidget::HandleEquipmentMessage);
	
	RefreshFromCurrentWeapon();
}

void UFPSCombatAmmoWidget::NativeDestruct()
{
	EquipmentListenerHandle.Unregister();
	
	Super::NativeDestruct();
}

void UFPSCombatAmmoWidget::HandleEquipmentMessage(FGameplayTag Channel, const FFPSCombatEquipmentChangedMessage& Message)
{
	RefreshFromCurrentWeapon();
}

void UFPSCombatAmmoWidget::RefreshFromCurrentWeapon()
{
	UFPSCombatRangedWeaponInstance* RangedWeaponInstance = nullptr;

	
	if (APawn* Pawn = GetOwningPlayerPawn())
	{
		if (UFPSCombatEquipmentManager* EqpManager = Pawn->FindComponentByClass<UFPSCombatEquipmentManager>())
		{
			RangedWeaponInstance = Cast<UFPSCombatRangedWeaponInstance>(EqpManager->GetFirstInstanceOfType(UFPSCombatRangedWeaponInstance::StaticClass()));
		}
	}

	if (RangedWeaponInstance)
	{
		SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else
	{
		SetVisibility(ESlateVisibility::Collapsed);
	}
}
