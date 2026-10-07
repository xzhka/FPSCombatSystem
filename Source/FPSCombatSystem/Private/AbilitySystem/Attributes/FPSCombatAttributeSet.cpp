// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Attributes/FPSCombatAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"



UFPSCombatAttributeSet::UFPSCombatAttributeSet()
	: Health(100.0f),
	  MaxHealth(100.0f),
	  Stamina(100.0f),
	  MaxStamina(100.0f),
	  MoveSpeed(1.f)
{
	bOutOfHealth = false;
	bOutOfStamina = false;
}

void UFPSCombatAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UFPSCombatAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UFPSCombatAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UFPSCombatAttributeSet, Stamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UFPSCombatAttributeSet, MaxStamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UFPSCombatAttributeSet, MoveSpeed, COND_None, REPNOTIFY_Always);
}


void UFPSCombatAttributeSet::HandleStaminaChange(float OldValue, float NewValue)
{
	if (FMath::IsNearlyEqual(OldValue, NewValue))
	{
		return;
	}
	
	OnStaminaChanged.Broadcast(OldValue, NewValue);

	// Broadcast on stamina changing
	const bool bIsNowOutOfStamina = (NewValue <= 0.0f);
	if (bIsNowOutOfStamina && !bOutOfStamina)
	{
		OnStaminaDepleted.Broadcast();
	}
	if (!bIsNowOutOfStamina && bOutOfStamina)
	{
		OnStaminaRestored.Broadcast();
	}
	bOutOfStamina = bIsNowOutOfStamina;
}

void UFPSCombatAttributeSet::OnRep_HealthChanged(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFPSCombatAttributeSet, Health, OldValue);

	const float CurrentHealth = GetHealth(); 
	
	
	OnHealthChanged.Broadcast(nullptr, nullptr, OldValue.GetCurrentValue(), CurrentHealth);

	// Broadcast on health changed to zero
	if (!bOutOfHealth && CurrentHealth <= 0.0f)
	{
		OnOutOfHealthChanged.Broadcast(nullptr, nullptr, OldValue.GetCurrentValue(), CurrentHealth);
	}

	bOutOfHealth = (CurrentHealth <= 0.0f);
}

void UFPSCombatAttributeSet::OnRep_MaxHealthChanged(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFPSCombatAttributeSet, MaxHealth, OldValue);

	OnMaxHealthChanged.Broadcast(nullptr, nullptr, OldValue.GetCurrentValue(), GetMaxHealth());
}

void UFPSCombatAttributeSet::OnRep_StaminaChanged(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFPSCombatAttributeSet, Stamina, OldValue);
	
	HandleStaminaChange(OldValue.GetCurrentValue(), GetStamina());
}

void UFPSCombatAttributeSet::OnRep_MaxStaminaChanged(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFPSCombatAttributeSet, MaxStamina, OldValue);

	OnMaxStaminaChanged.Broadcast(OldValue.GetCurrentValue(), GetMaxStamina());
}

void UFPSCombatAttributeSet::OnRep_MoveSpeedChanged(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFPSCombatAttributeSet, MoveSpeed, OldValue);
}

bool UFPSCombatAttributeSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	if(!Super::PreGameplayEffectExecute(Data))
	{
		return false;
	}

	if (bOutOfHealth && (Data.EvaluatedData.Attribute == GetHealAttribute() || Data.EvaluatedData.Attribute == GetDamageAttribute()))
	{
		return false;
	}

	return true;
}


void UFPSCombatAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	const float MinimumHealth = 0.0f;

	const FGameplayEffectContextHandle& EffectContext = Data.EffectSpec.GetEffectContext();
	const FGameplayEffectSpec* Spec = &Data.EffectSpec;
	AActor* InstigatorActor = EffectContext.GetOriginalInstigator();
	
	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		const float OldHealthValue = GetHealth();
		const float DamageDone = GetDamage();
		SetDamage(0.0f);
		
		if (DamageDone > 0.0f)
		{
			const float NewValue = FMath::Clamp(OldHealthValue - DamageDone, MinimumHealth, GetMaxHealth());
			SetHealth(NewValue);

			OnHealthChanged.Broadcast(InstigatorActor, Spec, OldHealthValue, NewValue);
			if ((NewValue <= 0.0f) && !bOutOfHealth)
			{
				bOutOfHealth = true;
				OnOutOfHealthChanged.Broadcast(InstigatorActor, Spec, OldHealthValue, NewValue);
			}
		}
	}
	else if (Data.EvaluatedData.Attribute == GetHealAttribute())
	{
		const float OldHealthValue = GetHealth();
		const float HealDone = GetHeal();
		SetHeal(0.0f);
		
		if (HealDone > 0.0f)
		{
			const float NewValue = FMath::Clamp(OldHealthValue + HealDone, MinimumHealth, GetMaxHealth());
			SetHealth(NewValue);
			
			if (NewValue > 0.0f)
			{
				bOutOfHealth = false;
			}
			OnHealthChanged.Broadcast(InstigatorActor, Spec, OldHealthValue, NewValue);
		}
	}
	else if (Data.EvaluatedData.Attribute == GetMaxHealthAttribute())
	{
		OnMaxHealthChanged.Broadcast(EffectContext.GetOriginalInstigator(), &Data.EffectSpec, 0.0f, GetMaxHealth());

		const float OldHealthValue = GetHealth();
		const float ClampedValue = FMath::Clamp(OldHealthValue, MinimumHealth, GetMaxHealth());
		if (ClampedValue != OldHealthValue)
		{
			SetHealth(ClampedValue);
			OnHealthChanged.Broadcast(InstigatorActor, Spec, OldHealthValue, ClampedValue);
		}
	}
	else if (Data.EvaluatedData.Attribute == GetStaminaAttribute())
	{
		HandleStaminaChange(StaminaBeforeChange, GetStamina());
	}
	else if (Data.EvaluatedData.Attribute == GetMaxStaminaAttribute())
	{
		OnMaxStaminaChanged.Broadcast(0.0f, GetMaxStamina());

		const float OldStamina = GetStamina();
		const float ClampStamina = FMath::Clamp(OldStamina, 0.0f, GetMaxStamina());
		if (OldStamina != ClampStamina)
		{
			SetStamina(ClampStamina);
			OnStaminaChanged.Broadcast(OldStamina, ClampStamina);
		}
	}
}

void UFPSCombatAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetHealthAttribute())
	{
		NewValue = GetClampToMax(NewValue, GetMaxHealth());
	}
	else if (Attribute == GetMaxHealthAttribute())
	{
		// We do not allow to MaxHealth value drop under 1
		NewValue = GetClampToMax(NewValue, GetMaxHealth());
	}

	if (Attribute == GetStaminaAttribute())
	{
		StaminaBeforeChange = GetStamina();
		NewValue = GetClampToMax(NewValue, GetMaxStamina());
	}
	else if (Attribute == GetMaxStaminaAttribute())
	{
		// As a Health we do not allow to MaxStamina drop below max stamina
		NewValue = GetClampToMax(NewValue, GetMaxStamina());
	}

	if (Attribute == GetMoveSpeedAttribute())
	{
		constexpr float MaxValue = 3.f;
		NewValue = GetClampToMax(NewValue, MaxValue);
	}
}

void UFPSCombatAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);

	if (Attribute == GetHealthAttribute())
	{
		NewValue = GetClampToMax(NewValue, GetMaxHealth());
	}
	else if (Attribute == GetStaminaAttribute())
	{
		NewValue = GetClampToMax(NewValue, GetMaxStamina());
	}
}

void UFPSCombatAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	if (bOutOfHealth && (GetHealth() > 0))
	{
		bOutOfHealth = false;
	}
}
