// FPS Combat project

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "AbilitySystem/FPSCombatAbilityTypes.h"
#include "FPSCombatBaseGameplayAbility.generated.h"


/** UFPSCombatBaseGameplayAbility
 *
 * Base setup parent ability for
 * ability classes
 */
UCLASS(Blueprintable)
class FPSCOMBATSYSTEM_API UFPSCombatBaseGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:

	UFPSCombatBaseGameplayAbility();
	
	virtual const FGameplayTagContainer* GetCooldownTags() const override;
	virtual void ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo) const override;
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr,
		FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual bool DoesAbilitySatisfyTagRequirements(const UAbilitySystemComponent& AbilitySystemComponent,
		const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr,
		FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	void TryActivateAbilityOnSpawn(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) const;

	virtual void NotifyInputReleased(const FGameplayAbilitySpec& Spec) {};
	
	virtual EFPSCombatAbilityActivationPolicy GetActivationPolicy(const FGameplayAbilitySpec& Spec) const { return ActivationPolicy; }

	virtual void OnPawnAvatarSet();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	AController* GetController();
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	UPROPERTY(EditDefaultsOnly, Category = "Tags")
	FGameplayTagContainer AddedOnTags;

	UPROPERTY(EditDefaultsOnly, Category = "Tags")
	FGameplayTagContainer RemovedOnTags;
	
	UFUNCTION()
	void HandleCancelTagTriggered();
	

	UPROPERTY(EditDefaultsOnly, Category = "Activation")
	FGameplayTagQuery ActivationTagQuery;

	UFUNCTION(BlueprintCallable)
	void EffectSpecApply(TArray<TSubclassOf<UGameplayEffect>> GrantedEffectsClass, TArray<FActiveGameplayEffectHandle>& GrantedEffectHandle, FGameplayTag DurationSetByCallerTag = FGameplayTag(), float DurationValue = 0.f);

	UFUNCTION(BlueprintCallable)
	void EffectSpecRemove(TArray<FActiveGameplayEffectHandle> GrantedEffectHandle);
	
	UPROPERTY(Transient)
	FGameplayTagContainer TempCooldownTags;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category = "Cooldown")
	FScalableFloat CooldownDuration;
	
	UPROPERTY(EditDefaultsOnly, Category = "Activation")
	EFPSCombatAbilityActivationPolicy ActivationPolicy;
	
	UPROPERTY(EditDefaultsOnly, Category = "Cooldown")
	FGameplayTagContainer CooldownTags;
	
	UPROPERTY(EditDefaultsOnly, Category = "Cooldown")
	FGameplayTag SetByCallerTag;
};
