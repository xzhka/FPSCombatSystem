// FPS Combat project

#pragma once

#include "CoreMinimal.h"
#include "FPSCombatBaseComponent.h"
#include "AbilitySystem/Attributes/FPSCombatAttributeSet.h"
#include "Components/ActorComponent.h"
#include "FPSCombatHealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFPSCombatDeathEvent, AActor*, OwningActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FFPSCombatAttributeChanged, AActor*, Investigator, float, OldValue, float, NewValue, UFPSCombatHealthComponent*, HealthComponent);

/** FFPSCombatDeathInfo
 *
 *	Struct which contain a death info
 *	that would be the place of truth
 *	for every client on server
 */
USTRUCT(BlueprintType)
struct FFPSCombatDeathInfo
{
GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly)
	int32 MontageIndex = INDEX_NONE;

	UPROPERTY(BlueprintReadOnly)
	float Alpha = 0.f;

	UPROPERTY(BlueprintReadOnly)
	FVector Velocity = FVector::ZeroVector;
};

/** EDeathState
 *
 *	Enum with current player state
 */
UENUM(BlueprintType)
enum class EDeathState : uint8
{
	NotDead = 0,
	DeathStarted,
	DeathEnded
};


/** UFPSCombatHealthComponent
 *
 *	Derived class which represents health stat
 *	and work with the death event triggering
 */
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
	UFUNCTION(BlueprintCallable, Category = "Components|Death")
	const FFPSCombatDeathInfo& GetDeathInfo() const { return DeathInfo; }

	void SetDeathInfo(const FFPSCombatDeathInfo& NewDeathInfo);
	
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

	UPROPERTY(Replicated)
	FFPSCombatDeathInfo DeathInfo;
	
	
};
