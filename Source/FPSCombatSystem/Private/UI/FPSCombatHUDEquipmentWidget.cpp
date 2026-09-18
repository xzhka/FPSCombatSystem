// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/FPSCombatHUDEquipmentWidget.h"

#include "Equipment/FPSCombatEquipmentInstance.h"
#include "Equipment/FPSCombatEquipmentManager.h"
#include "FPSCombatSystem/FPSCombatGameplayTags.h"

void UFPSCombatHUDEquipmentWidget::SetVisibility(ESlateVisibility InVisibility)
{
	if (IsDesignTime())
	{
		Super::SetVisibility(InVisibility);
		return;
	}

	bWantsToVisible = ConvertSerializedVisibilityToRuntime(InVisibility).IsVisible();
	if (bWantsToVisible)
	{
		ShownVisibility = InVisibility;
	}
	else
	{
		HiddenVisibility = InVisibility;
	}

	const ESlateVisibility DesiredVisibility = bWantsToVisible ? ShownVisibility : HiddenVisibility;
	if (GetVisibility() != DesiredVisibility)
	{
		Super::SetVisibility(DesiredVisibility);
	}
}

void UFPSCombatHUDEquipmentWidget::NativeConstruct()
{
	Super::NativeConstruct();

	EquipmentListenerHandle = UGameplayMessageSubsystem::Get(this).RegisterListener(FPSCombatGameplayTags::Message_Equipment_Change, this, &UFPSCombatHUDEquipmentWidget::HandleEquipmentMessage);
}

void UFPSCombatHUDEquipmentWidget::NativeDestruct()
{
	EquipmentListenerHandle.Unregister();
	
	Super::NativeDestruct();
}

UFPSCombatEquipmentInstance* UFPSCombatHUDEquipmentWidget::GetWatchedInstance() const
{
	if (!WatchedInstanceType) return nullptr;

	if (APawn* Pawn = GetOwningPlayerPawn())
	{
		if (UFPSCombatEquipmentManager* EqpManager = Pawn->FindComponentByClass<UFPSCombatEquipmentManager>())
		{
			return EqpManager->GetFirstInstanceOfType(WatchedInstanceType);
		}
	}
	return nullptr;
}

void UFPSCombatHUDEquipmentWidget::RefreshWatchedInstance()
{
	UFPSCombatEquipmentInstance* EquipmentInstance = nullptr;

	
	if (APawn* Pawn = GetOwningPlayerPawn())
	{
		if (UFPSCombatEquipmentManager* EqpManager = Pawn->FindComponentByClass<UFPSCombatEquipmentManager>())
		{
			EquipmentInstance = EqpManager->GetFirstInstanceOfType(WatchedInstanceType);
		}
	}
	
	if (EquipmentInstance)
	{
		SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else
	{
		SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UFPSCombatHUDEquipmentWidget::HandleEquipmentMessage(FGameplayTag Channel,
                                                          const FFPSCombatEquipmentChangedMessage& Message)
{
	RefreshWatchedInstance();
}
