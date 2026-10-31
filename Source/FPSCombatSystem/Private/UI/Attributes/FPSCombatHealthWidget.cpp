// FPS Combat project


#include "UI/Attributes/FPSCombatHealthWidget.h"

#include "Characters/FPSCombatCharacter.h"

void UFPSCombatHealthWidget::NativeDestruct()
{
	FOnPercentChanged::FDelegate H;
	H.BindDynamic(this, &UFPSCombatHealthWidget::HandleHealthPercentUpdated);
	RebindComp(HealthComponent, nullptr, H);
	
	Super::NativeDestruct();
}

void UFPSCombatHealthWidget::HandlePawnChanged(APawn* OldPawn, APawn* NewPawn)
{
	if (AFPSCombatCharacter* Character = Cast<AFPSCombatCharacter>(NewPawn))
	{
		FOnPercentChanged::FDelegate H;
		H.BindDynamic(this, &UFPSCombatHealthWidget::HandleHealthPercentUpdated);
		RebindComp(HealthComponent, Character->FindComponentByClass<UFPSCombatHealthComponent>(), H );
		if (HealthComponent.IsValid())
		{
			HandleHealthPercentUpdated(HealthComponent->GetMerged());
		}
	}
}
