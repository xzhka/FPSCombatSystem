// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Components/FPSCombatHealthComponent.h"

#include "Net/UnrealNetwork.h"


UFPSCombatHealthComponent::UFPSCombatHealthComponent()
{
	SetIsReplicatedByDefault(true);

	AbilitySystem = nullptr;
	HealthSet = nullptr;
	DeathState = EDeathState::NotDead;
}

void UFPSCombatHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UFPSCombatHealthComponent, DeathState);
}

void UFPSCombatHealthComponent::InitializeWithAbilitySystem(UFPSCombatAbilitySystemComponent* ASC)
{
	AActor* OwningActor = GetOwner();

	check(OwningActor);

	AbilitySystem = ASC;

	if (!AbilitySystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot initialize health component for owner %s"), *GetNameSafe(OwningActor));
		return;
	}

	HealthSet = AbilitySystem->GetSet<UFPSCombatAttributeSet>();

	if (!HealthSet)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot initialize health set for owner %s"), *GetNameSafe(OwningActor));
		return;
	}

	HealthSet->OnHealthChanged.AddUObject(this, &ThisClass::HandleHealthChanged);
	HealthSet->OnMaxHealthChanged.AddUObject(this, &ThisClass::HandleMaxHealthChanged);
	HealthSet->OnOutOfHealthChanged.AddUObject(this, &ThisClass::HandleOutOfHealthChanged);

	AbilitySystem->SetNumericAttributeBase(UFPSCombatAttributeSet::GetHealthAttribute(), HealthSet->GetMaxHealth());

	ClearASCGameplayTags();

	OnHealthChanged.Broadcast(nullptr, HealthSet->GetHealth(), HealthSet->GetHealth(), this);
	OnMaxHealthChanged.Broadcast(nullptr, HealthSet->GetHealth(), HealthSet->GetHealth(), this);
}

void UFPSCombatHealthComponent::UninitializeFromAbilitySystem()
{
	ClearASCGameplayTags();
	if (HealthSet)
	{
		HealthSet->OnHealthChanged.RemoveAll(this);
		HealthSet->OnMaxHealthChanged.RemoveAll(this);
		HealthSet->OnOutOfHealthChanged.RemoveAll(this);
	}

	HealthSet = nullptr;
	AbilitySystem = nullptr;
}

float UFPSCombatHealthComponent::GetHealth() const
{
	return (HealthSet ? HealthSet->GetHealth() : 0.f);
}

float UFPSCombatHealthComponent::GetMaxHealth() const
{
	return (HealthSet ? HealthSet->GetMaxHealth() : 0.f);
}

float UFPSCombatHealthComponent::GetMergedHealth() const
{
	if (HealthSet)
	{
		const float Health = HealthSet->GetHealth();
		const float MaxHealth = HealthSet->GetMaxHealth();

		return ((MaxHealth > 0.f) ? (Health / MaxHealth) : 0.f);
	}
	return 0.f;
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
			DeathEnded();
		}
		else if (NewDeathState == EDeathState::DeathEnded)
		{
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
		Payload.EventMagnitude = NULL;
		
		AbilitySystem->HandleGameplayEvent(Payload.EventTag, &Payload);
	}
}







