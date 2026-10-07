// FPS Combat project


#include "UI/FPSCombatHUDEquipmentWidget.h"

#include "Equipment/FPSCombatEquipmentInstance.h"
#include "Equipment/FPSCombatEquipmentManager.h"
#include "FPSCombatSystem/FPSCombatGameplayTags.h"

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
