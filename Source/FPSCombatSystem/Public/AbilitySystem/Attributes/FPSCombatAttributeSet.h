// FPS Combat project

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "NativeGameplayTags.h"

#include "FPSCombatAttributeSet.generated.h"

struct FGameplayEffectSpec;

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)


DECLARE_MULTICAST_DELEGATE_FourParams(FFPSAttributeHealthEvent, AActor* /*EffectInstigator*/, const FGameplayEffectSpec* /*EffectSpec*/, float /*OldValue*/, float /*NewValue*/);
DECLARE_MULTICAST_DELEGATE(FFPSAttributeStaminaEvent);
DECLARE_MULTICAST_DELEGATE_TwoParams(FFPSAttributeStaminaChangedEvent, float /*OldValue*/, float /*NewValue*/);

/* UFPSCombatAttributeSet
 * 
 * Attribute class declaration for stats 
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
	
	mutable FFPSAttributeStaminaEvent OnStaminaDepleted;
	mutable FFPSAttributeStaminaEvent OnStaminaRestored;
	mutable FFPSAttributeStaminaChangedEvent OnStaminaChanged;
	mutable FFPSAttributeStaminaChangedEvent OnMaxStaminaChanged;
	
	/* Initialize the attribute access macro */
	ATTRIBUTE_ACCESSORS(UFPSCombatAttributeSet, Health);
	ATTRIBUTE_ACCESSORS(UFPSCombatAttributeSet, MaxHealth);
	ATTRIBUTE_ACCESSORS(UFPSCombatAttributeSet, Heal);
	ATTRIBUTE_ACCESSORS(UFPSCombatAttributeSet, Damage);

	ATTRIBUTE_ACCESSORS(UFPSCombatAttributeSet, Stamina);
	ATTRIBUTE_ACCESSORS(UFPSCombatAttributeSet, MaxStamina);

	ATTRIBUTE_ACCESSORS(UFPSCombatAttributeSet, MoveSpeed);

	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	
private:
	/* Create a replicate attribute data for stats */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HealthChanged, Category = "Attributes|Health", meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Health;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealthChanged, Category = "Attributes|Health", meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxHealth;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_StaminaChanged, Category = "Attributes|Stamina", meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Stamina;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxStaminaChanged, Category = "Attributes|Stamina", meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxStamina;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing= OnRep_MoveSpeedChanged, Category = "Attributes|Speed" , meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MoveSpeed;
	
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Health", meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Heal;

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Damage", Meta = (HideFromModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData Damage;
	
	FORCEINLINE float GetClampToMax(float Value, float MaxValue) const { return FMath::Clamp(Value, 0.f, MaxValue); }

	void HandleStaminaChange(float OldValue, float NewValue);

	bool bOutOfHealth = false;

	bool bOutOfStamina = false;

	float StaminaBeforeChange = 0.f;
	
protected:
	/* Replication using functions */
	UFUNCTION()
	void OnRep_HealthChanged(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxHealthChanged(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_StaminaChanged(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxStaminaChanged(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MoveSpeedChanged(const FGameplayAttributeData& OldValue);

public:
	/* UAttributeSet override functions */
	virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
};
