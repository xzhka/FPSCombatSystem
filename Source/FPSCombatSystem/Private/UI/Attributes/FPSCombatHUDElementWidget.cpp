// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Attributes/FPSCombatHUDElementWidget.h"

void UFPSCombatHUDElementWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (APlayerController* PC = GetOwningPlayer())
	{
		HandlePawnChanged(nullptr, PC->GetPawn());
		PC->OnPossessedPawnChanged.AddDynamic(this, &UFPSCombatHUDElementWidget::OnPossessedPawnChangedInternal);
	}
}

void UFPSCombatHUDElementWidget::NativeDestruct()
{
	if (APlayerController* PC = GetOwningPlayer())
	{
		PC->OnPossessedPawnChanged.RemoveDynamic(this, &UFPSCombatHUDElementWidget::OnPossessedPawnChangedInternal);
	}
	Super::NativeDestruct();
}

void UFPSCombatHUDElementWidget::RebindComp(TWeakObjectPtr<UFPSCombatBaseComponent>& BaseComp,
	UFPSCombatBaseComponent* Component, FOnPercentChanged::FDelegate Delegate)
{
	if (BaseComp.IsValid())
	{
		BaseComp->OnPercentChanged.Remove(Delegate);
	}

	BaseComp = Component;

	if (BaseComp.IsValid())
	{
		BaseComp->OnPercentChanged.Add(Delegate);
	}
}
