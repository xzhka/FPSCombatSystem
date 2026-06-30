// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FPSCombatSystem/FPSCombatGameplayTags.h"
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
class FPSCOMBATSYSTEM_API UFPSCombatHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UFPSCombatHealthComponent();


	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Category = "Components|Health")
	void InitializeWithAbilitySystem(UFPSCombatAbilitySystemComponent* ASC);


	UFUNCTION(BlueprintCallable, Category = "Components|Health")
	void UninitializeFromAbilitySystem();
	
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
	

protected:
	UPROPERTY()
	TObjectPtr<UFPSCombatAbilitySystemComponent> AbilitySystem;

	UPROPERTY(ReplicatedUsing= OnRep_DeathStateChange)
	EDeathState DeathState;
	
	UPROPERTY()
	TObjectPtr<const UFPSCombatAttributeSet> HealthSet;
	
};
