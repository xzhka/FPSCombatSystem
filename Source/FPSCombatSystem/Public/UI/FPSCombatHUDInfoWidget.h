// FPS Combat project

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FPSCombatHUDInfoWidget.generated.h"

/** UFPSCombatHUDInfoWidget
 *
 *	Parent class for info widgets
 */
UCLASS(Abstract)
class FPSCOMBATSYSTEM_API UFPSCombatHUDInfoWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void SetVisibility(ESlateVisibility InVisibility) override;	

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	UPROPERTY(EditAnywhere, Category = "HUD")
	ESlateVisibility ShownVisibility = ESlateVisibility::Visible;
	
	UPROPERTY(EditAnywhere, Category = "HUD")
	ESlateVisibility HiddenVisibility = ESlateVisibility::Collapsed;
private:

	bool bWantsToVisible = true;
	
};
