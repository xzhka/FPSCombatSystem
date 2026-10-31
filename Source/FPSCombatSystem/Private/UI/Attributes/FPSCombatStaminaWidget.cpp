// FPS Combat project


#include "UI/Attributes/FPSCombatStaminaWidget.h"

#include "Characters/FPSCombatCharacter.h"

void UFPSCombatStaminaWidget::NativeDestruct()
{
	FOnPercentChanged::FDelegate S;
	S.BindDynamic(this, &UFPSCombatStaminaWidget::HandleStaminaPercentUpdated);
	RebindComp(StaminaComponent, nullptr, S);

	
	Super::NativeDestruct();
}

void UFPSCombatStaminaWidget::HandlePawnChanged(APawn* OldPawn, APawn* NewPawn)
{
	if (AFPSCombatCharacter* Character = Cast<AFPSCombatCharacter>(NewPawn))
	{
		FOnPercentChanged::FDelegate S;
		S.BindDynamic(this, &UFPSCombatStaminaWidget::HandleStaminaPercentUpdated);
		RebindComp(StaminaComponent, Character->FindComponentByClass<UFPSCombatStaminaComponent>(), S );
		if (StaminaComponent.IsValid())
		{
			HandleStaminaPercentUpdated(StaminaComponent->GetMerged());
		}
	}
	
}
