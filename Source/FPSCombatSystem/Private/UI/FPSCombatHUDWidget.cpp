// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/FPSCombatHUDWidget.h"

#include "Characters/FPSCombatCharacter.h"

void UFPSCombatHUDWidget::RebindComp(TWeakObjectPtr<UFPSCombatBaseComponent>& BaseComp,
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

void UFPSCombatHUDWidget::NativeConstruct()
{

	if (APlayerController* PC = GetOwningPlayer())
	{
		HandlePawnChanged(nullptr, PC->GetPawn());
		PC->OnPossessedPawnChanged.AddDynamic(this, &UFPSCombatHUDWidget::HandlePawnChanged);
	}
	
	Super::NativeConstruct();
}

void UFPSCombatHUDWidget::NativeDestruct()
{
	FOnPercentChanged::FDelegate H;
	H.BindDynamic(this, &UFPSCombatHUDWidget::HandleHealthPercentUpdated);
	RebindComp(HealthComponent, nullptr, H);

	FOnPercentChanged::FDelegate S;
	S.BindDynamic(this, &UFPSCombatHUDWidget::HandleStaminaPercentUpdated);
	RebindComp(StaminaComponent, nullptr, S);
	
	Super::NativeDestruct();
}

void UFPSCombatHUDWidget::HandlePawnChanged(APawn* OldPawn, APawn* NewPawn)
{
	if (AFPSCombatCharacter* Character = Cast<AFPSCombatCharacter>(NewPawn))
	{
		FOnPercentChanged::FDelegate H;
		H.BindDynamic(this, &UFPSCombatHUDWidget::HandleHealthPercentUpdated);
		RebindComp(HealthComponent, Character->FindComponentByClass<UFPSCombatHealthComponent>(), H );
		if (HealthComponent.IsValid())
		{
			HandleHealthPercentUpdated(HealthComponent->GetMerged());
		}

		
		FOnPercentChanged::FDelegate S;
		S.BindDynamic(this, &UFPSCombatHUDWidget::HandleStaminaPercentUpdated);
		RebindComp(StaminaComponent, Character->FindComponentByClass<UFPSCombatStaminaComponent>(), S );
		if (StaminaComponent.IsValid())
		{
			HandleStaminaPercentUpdated(StaminaComponent->GetMerged());
		}
	}
}