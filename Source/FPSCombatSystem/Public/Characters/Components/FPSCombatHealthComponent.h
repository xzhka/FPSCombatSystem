// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FPSCombatBaseComponent.h"
#include "AbilitySystem/FPSCombatAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/FPSCombatAttributeSet.h"
#include "Components/ActorComponent.h"
#include "FPSCombatHealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFPSCombatDeathEvent, AActor*, OwningActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FFPSCombatAttributeChanged, AActor*, Investigator, float, OldValue, float, NewValue, UFPSCombatHealthComponent*, HealthComponent);

UENUM(BlueprintType)
enum class EDeathState : uint8
{
	NotDead = 0,
	DeathStarted,
	DeathEnded
};



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FPSCOMBATSYSTEM_API UFPSCombatHealthComponent : public UFPSCombatBaseComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UFPSCombatHealthComponent();


	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintPure)
	static UFPSCombatHealthComponent* GetHealthComp(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UFPSCombatHealthComponent>() : nullptr); }
	
	UFUNCTION(BlueprintCallable, Category = "Components|Health")
	float GetHealth() const;
	
	UFUNCTION(BlueprintCallable, Category = "Components|Health")
	float GetMaxHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Components|Health")
	float GetMergedHealth() const;
	

	UFUNCTION(BlueprintCallable, Category = "Components|Health")
	EDeathState GetDeathState() const { return DeathState; };

	
	UPROPERTY(BlueprintAssignable)
	FFPSCombatDeathEvent OnDeathStarted;

	UPROPERTY(BlueprintAssignable)
	FFPSCombatDeathEvent OnDeathEnded;
	

	UPROPERTY(BlueprintAssignable)
	FFPSCombatAttributeChanged OnHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FFPSCombatAttributeChanged OnMaxHealthChanged;

	virtual void DeathStarted();
	
	virtual void DeathEnded();
	

	UFUNCTION(BlueprintCallable, BlueprintPure = false ,Category = "Components|Death", meta = (ExpandBoolAsExecs = "ReturnValue"))
	bool IsDeadOrDying() const { return (DeathState > EDeathState::NotDead);   }
	
	
protected:

	UFUNCTION()
	virtual void OnRep_DeathStateChange(EDeathState OldDeathState);

	void ClearASCGameplayTags();
	
	
	virtual void HandleHealthChanged(AActor* EffectInstigator, const FGameplayEffectSpec* EffectSpec, float OldValue, float NewValue);
	virtual void HandleMaxHealthChanged(AActor* EffectInstigator, const FGameplayEffectSpec* EffectSpec, float OldValue, float NewValue);
	virtual void HandleOutOfHealthChanged(AActor* EffectInstigator, const FGameplayEffectSpec* EffectSpec, float OldValue, float NewValue);
	virtual void BindAttributeDelegate() override;
	virtual void UnBindAttributeDelegate() override;
	virtual float GetMerged() override { return GetMergedHealth(); };


	UPROPERTY(ReplicatedUsing= OnRep_DeathStateChange)
	EDeathState DeathState;
	
	
};
