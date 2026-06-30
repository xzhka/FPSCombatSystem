// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/FPSCombatHeathBarWidget.h"

#include "Characters/FPSCombatCharacter.h"
#include "Input/FPSCombatPlayerController.h"

void UFPSCombatHeathBarWidget::InitializeWithHealthComponent(UFPSCombatHealthComponent* HC)
{

	if (HealthComponent.IsValid())
	{
		HealthComponent->OnHealthChanged.RemoveDynamic(this, &UFPSCombatHeathBarWidget::HandleHealthUpdated);
	}
	
	HealthComponent = HC;
	
	if (HealthComponent.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Initialize with Health Component function is called and Health is %f and MaxHealth is %f"), HealthComponent->GetHealth(), HealthComponent->GetMaxHealth());
		HealthComponent->OnHealthChanged.AddDynamic(this, &UFPSCombatHeathBarWidget::HandleHealthUpdated);
		OnHealthUpdated(HealthComponent->GetMergedHealth());
	}
}

void UFPSCombatHeathBarWidget::HandleHealthUpdated(AActor* Instigator, float OldValue, float NewValue,
	UFPSCombatHealthComponent* HC)
{
	OnHealthUpdated(HealthComponent->GetMergedHealth());
}

void UFPSCombatHeathBarWidget::NativeConstruct()
{

	if (APlayerController* PC = GetOwningPlayer())
	{
		if (AFPSCombatCharacter* AC = Cast<AFPSCombatCharacter>(PC->GetPawn()))
		{
			InitializeWithHealthComponent(AC->FindComponentByClass<UFPSCombatHealthComponent>());
		}
		PC->OnPossessedPawnChanged.AddDynamic(this, &UFPSCombatHeathBarWidget::HandlePawnChanged);
	}
	
	Super::NativeConstruct();
}

void UFPSCombatHeathBarWidget::NativeDestruct()
{
	if (HealthComponent.IsValid())
	{
		HealthComponent->OnHealthChanged.RemoveDynamic(this, &UFPSCombatHeathBarWidget::HandleHealthUpdated);
	}
	
	Super::NativeDestruct();
}

void UFPSCombatHeathBarWidget::HandlePawnChanged(APawn* OldPawn, APawn* NewPawn)
{
	UFPSCombatHealthComponent* HealthComp = nullptr;
	if (AFPSCombatCharacter* Character = Cast<AFPSCombatCharacter>(NewPawn))
	{
		HealthComp = Character->FindComponentByClass<UFPSCombatHealthComponent>();
	}
	InitializeWithHealthComponent(HealthComp);
}