// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/FPSCombatInfoWidget.h"

void UFPSCombatInfoWidget::SetVisibility(ESlateVisibility InVisibility)
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

void UFPSCombatInfoWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UFPSCombatInfoWidget::NativeDestruct()
{
	Super::NativeDestruct();
}
