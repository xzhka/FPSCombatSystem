// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Attributes/FPSCombatAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

UE_DEFINE_GAMEPLAY_TAG(GameplayTag_Damage, "Gameplay.Damage");


UFPSCombatAttributeSet::UFPSCombatAttributeSet()
	: Health(100.0f),
	  MaxHealth(100.0f)
{
	bOutOfHealth = false;
}

void UFPSCombatAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UFPSCombatAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UFPSCombatAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
}

void UFPSCombatAttributeSet::OnRep_HealthChanged(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFPSCombatAttributeSet, Health, OldValue);

	const float CurrentHealth = GetHealth(); 
	
	OnHealthChanged.Broadcast(nullptr, nullptr, OldValue.GetCurrentValue(), CurrentHealth);

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
		SetDamage(0.0f);
		
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
}

void UFPSCombatAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}
	else if (Attribute == GetMaxHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}
}

