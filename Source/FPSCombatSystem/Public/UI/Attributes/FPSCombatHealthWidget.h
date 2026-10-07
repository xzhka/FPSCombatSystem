// FPS Combat project

#pragma once

#include "CoreMinimal.h"
#include "UI/Attributes/FPSCombatHUDElementWidget.h"
#include "FPSCombatHealthWidget.generated.h"

/** UFPSCombatHealthWidget
 *
 *	Widget class connected with health component
 */
UCLASS()
class FPSCOMBATSYSTEM_API UFPSCombatHealthWidget : public UFPSCombatHUDElementWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Health")
	TWeakObjectPtr<UFPSCombatBaseComponent> HealthComponent;

	UFUNCTION(BlueprintImplementableEvent)
	void OnHealthUpdated(float Percent);
	
	UFUNCTION()
	void HandleHealthPercentUpdated(const float Percent) { OnHealthUpdated(Percent); }
	
	virtual void NativeDestruct() override;
	virtual void HandlePawnChanged(APawn* OldPawn, APawn* NewPawn) override;
};
