// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "NativeGameplayTags.h"

#include "FPSCombatAttributeSet.generated.h"

struct FGameplayEffectSpec;

UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayTag_Damage);



#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)


DECLARE_MULTICAST_DELEGATE_FourParams(FFPSAttributeHealthEvent, AActor* /*EffectInstigator*/, const FGameplayEffectSpec* /*EffectSpec*/, float /*OldValue*/, float /*NewValue*/);

/**
 * 
 */
UCLASS()
class FPSCOMBATSYSTEM_API UFPSCombatAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UFPSCombatAttributeSet();

	mutable FFPSAttributeHealthEvent OnHealthChanged;
	
	mutable FFPSAttributeHealthEvent OnMaxHealthChanged;
	
	mutable FFPSAttributeHealthEvent OnOutOfHealthChanged;
	
	
	ATTRIBUTE_ACCESSORS(UFPSCombatAttributeSet, Health);
	ATTRIBUTE_ACCESSORS(UFPSCombatAttributeSet, MaxHealth);
	ATTRIBUTE_ACCESSORS(UFPSCombatAttributeSet, Heal);
	ATTRIBUTE_ACCESSORS(UFPSCombatAttributeSet, Damage);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	
private:
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HealthChanged, Category = "Attributes|Health", meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Health;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealthChanged, Category = "Attributes|Health", meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxHealth;
	
	bool bOutOfHealth = false;

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Health", meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Heal;

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Health", Meta = (HideFromModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData Damage;
	
		
	
protected:
	UFUNCTION()
	void OnRep_HealthChanged(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxHealthChanged(const FGameplayAttributeData& OldValue);

public:
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
};
