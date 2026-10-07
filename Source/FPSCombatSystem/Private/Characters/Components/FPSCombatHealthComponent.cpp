// FPS Combat project


#include "Characters/Components/FPSCombatHealthComponent.h"

#include "FPSCombatSystem/FPSCombatGameplayTags.h"
#include "Net/UnrealNetwork.h"


UFPSCombatHealthComponent::UFPSCombatHealthComponent()
{
	SetIsReplicatedByDefault(true);

	AbilitySystem = nullptr;
	AttributeSet = nullptr;
	DeathState = EDeathState::NotDead;
}

void UFPSCombatHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UFPSCombatHealthComponent, DeathState);
	DOREPLIFETIME(UFPSCombatHealthComponent, DeathInfo);
}


float UFPSCombatHealthComponent::GetHealth() const
{
	return (AttributeSet ? AttributeSet->GetHealth() : 0.f);
}

float UFPSCombatHealthComponent::GetMaxHealth() const
{
	return (AttributeSet ? AttributeSet->GetMaxHealth() : 0.f);
}

float UFPSCombatHealthComponent::GetMergedHealth() const
{
	if (AttributeSet)
	{
		const float Health = AttributeSet->GetHealth();
		const float MaxHealth = AttributeSet->GetMaxHealth();

		return ((MaxHealth > 0.f) ? (Health / MaxHealth) : 0.f);
	}
	return 0.f;
}

void UFPSCombatHealthComponent::SetDeathInfo(const FFPSCombatDeathInfo& NewDeathInfo)
{
	if (GetOwner()->HasAuthority())
	{
		DeathInfo = NewDeathInfo;
	}
}


void UFPSCombatHealthComponent::DeathStarted()
{
	if (DeathState != EDeathState::NotDead)
	{
		return;
	}

	DeathState = EDeathState::DeathStarted;
	
	if (AbilitySystem)
	{
		AbilitySystem->SetLooseGameplayTagCount(FPSCombatGameplayTags::State_Death_Started,1);
	}

	AActor* OwningActor = GetOwner();

	check(OwningActor);

	OnDeathStarted.Broadcast(OwningActor);

	OwningActor->ForceNetUpdate();
	
}

void UFPSCombatHealthComponent::DeathEnded()
{
	if (DeathState != EDeathState::DeathStarted)
	{
		return;
	}

	DeathState = EDeathState::DeathEnded;

	if (AbilitySystem)
	{
		AbilitySystem->SetLooseGameplayTagCount(FPSCombatGameplayTags::State_Death_Ended,1);
	}

	AActor* OwningActor = GetOwner();

	check(OwningActor);
	
	
	OnDeathEnded.Broadcast(OwningActor);

	OwningActor->ForceNetUpdate();
	
}

void UFPSCombatHealthComponent::OnRep_DeathStateChange(EDeathState OldDeathState)
{
	const EDeathState NewDeathState = DeathState;

	DeathState = OldDeathState;
	if (OldDeathState > NewDeathState)
	{
		UE_LOG(LogTemp, Warning, TEXT("In Health Component predicted past state: old state [%hhu] to [%hhu]"), (uint8)OldDeathState, (uint8)NewDeathState);
		return;
	}

	if (OldDeathState == EDeathState::NotDead)
	{
		if (NewDeathState == EDeathState::DeathStarted)
		{
			DeathStarted();
		}
		else if (NewDeathState == EDeathState::DeathEnded)
		{
			DeathStarted();
			DeathEnded();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("In Health Component wrong transition: old state [%hhu] to [%hhu]"), (uint8)OldDeathState, (uint8)NewDeathState);
		}
	}
	else if (OldDeathState == EDeathState::DeathStarted)
	{
		if (NewDeathState == EDeathState::DeathEnded)
		{
			DeathEnded();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("In Health Component wrong transition: old state [%hhu] to [%hhu]"), (uint8)OldDeathState, (uint8)NewDeathState);
		}
	}
}

void UFPSCombatHealthComponent::ClearASCGameplayTags()
{
	if (AbilitySystem)
	{
		AbilitySystem->SetLooseGameplayTagCount(FPSCombatGameplayTags::State_Death_Started, 0);
		AbilitySystem->SetLooseGameplayTagCount(FPSCombatGameplayTags::State_Death_Ended, 0);
	}
}


void UFPSCombatHealthComponent::HandleHealthChanged(AActor* EffectInstigator, const FGameplayEffectSpec* EffectSpec,
                                                    float OldValue, float NewValue)
{
	OnHealthChanged.Broadcast(EffectInstigator, OldValue, NewValue, this);
	OnPercentChanged.Broadcast(GetMerged());
}

void UFPSCombatHealthComponent::HandleMaxHealthChanged(AActor* EffectInstigator, const FGameplayEffectSpec* EffectSpec,
	float OldValue, float NewValue)
{
	OnMaxHealthChanged.Broadcast(EffectInstigator, OldValue, NewValue, this);
}

void UFPSCombatHealthComponent::HandleOutOfHealthChanged(AActor* EffectInstigator,
	const FGameplayEffectSpec* EffectSpec, float OldValue, float NewValue)
{
	if (AbilitySystem && EffectSpec)
	{
		FGameplayEventData Payload;
		Payload.EventTag = FPSCombatGameplayTags::State_Death;
		Payload.Instigator = EffectInstigator;
		Payload.Target = AbilitySystem->GetAvatarActor();
		Payload.OptionalObject = EffectSpec->Def;
		Payload.ContextHandle = EffectSpec->GetEffectContext();
		Payload.InstigatorTags = *EffectSpec->CapturedSourceTags.GetAggregatedTags();
		Payload.TargetTags = *EffectSpec->CapturedTargetTags.GetAggregatedTags();
		Payload.EventMagnitude = 0.f;
		
		AbilitySystem->HandleGameplayEvent(Payload.EventTag, &Payload);
	}
}

void UFPSCombatHealthComponent::BindAttributeDelegate()
{
	AttributeSet->OnHealthChanged.AddUObject(this, &ThisClass::HandleHealthChanged);
	AttributeSet->OnMaxHealthChanged.AddUObject(this, &ThisClass::HandleMaxHealthChanged);
	AttributeSet->OnOutOfHealthChanged.AddUObject(this, &ThisClass::HandleOutOfHealthChanged);

	AbilitySystem->SetNumericAttributeBase(UFPSCombatAttributeSet::GetHealthAttribute(), AttributeSet->GetMaxHealth());

	ClearASCGameplayTags();

	HandleHealthChanged(nullptr, nullptr, AttributeSet->GetHealth(), AttributeSet->GetHealth());
	HandleMaxHealthChanged(nullptr, nullptr, AttributeSet->GetMaxHealth(), AttributeSet->GetMaxHealth());
}

void UFPSCombatHealthComponent::UnBindAttributeDelegate()
{
	ClearASCGameplayTags();

	AttributeSet->OnHealthChanged.RemoveAll(this);
	AttributeSet->OnMaxHealthChanged.RemoveAll(this);
	AttributeSet->OnOutOfHealthChanged.RemoveAll(this);
	
}
