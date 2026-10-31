// FPS Combat project

#pragma once

#include "CoreMinimal.h"
#include "FPSCombatHUDInfoWidget.h"
#include "FPSCombatSystem/FPSCombatMessageTypes.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "FPSCombatHUDEquipmentWidget.generated.h"

/** UFPSCombatHUDEquipmentWidget
 *
 *	Base class associated with equipment instance
 */
UCLASS()
class FPSCOMBATSYSTEM_API UFPSCombatHUDEquipmentWidget : public UFPSCombatHUDInfoWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	UPROPERTY(EditDefaultsOnly, Category = "HUD")
	TSubclassOf<UFPSCombatEquipmentInstance> WatchedInstanceType;
	
private:
	void RefreshWatchedInstance();
	void HandleEquipmentMessage(FGameplayTag Channel, const FFPSCombatEquipmentChangedMessage& Message);
	
	FGameplayMessageListenerHandle EquipmentListenerHandle;

	bool bWantsToVisible = true;
	
};
