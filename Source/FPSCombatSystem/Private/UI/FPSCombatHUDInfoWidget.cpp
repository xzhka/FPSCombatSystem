// FPS Combat project


#include "UI/FPSCombatHUDInfoWidget.h"

void UFPSCombatHUDInfoWidget::SetVisibility(ESlateVisibility InVisibility)
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

void UFPSCombatHUDInfoWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UFPSCombatHUDInfoWidget::NativeDestruct()
{
	Super::NativeDestruct();
}
