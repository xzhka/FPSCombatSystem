// FPS Combat project

#pragma once

#include "CoreMinimal.h"
#include "UI/Attributes/FPSCombatHUDElementWidget.h"
#include "FPSCombatStaminaWidget.generated.h"

/** UFPSCombatStaminaWidget
 *
 *	Widget class connected with stamina component
 */
UCLASS()
class FPSCOMBATSYSTEM_API UFPSCombatStaminaWidget : public UFPSCombatHUDElementWidget
{
	GENERATED_BODY()


protected:

	UPROPERTY(BlueprintReadOnly, Category = "Stamina")
	TWeakObjectPtr<UFPSCombatBaseComponent> StaminaComponent;

	UFUNCTION(BlueprintImplementableEvent)
	void OnStaminaUpdated(float Percent);
	
	UFUNCTION()
	void HandleStaminaPercentUpdated(const float Percent) { OnStaminaUpdated(Percent); }
	
	virtual void NativeDestruct() override;
	virtual void HandlePawnChanged(APawn* OldPawn, APawn* NewPawn) override;
	
};
