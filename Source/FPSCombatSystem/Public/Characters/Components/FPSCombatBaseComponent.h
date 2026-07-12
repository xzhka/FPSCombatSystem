// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/FPSCombatAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/FPSCombatAttributeSet.h"
#include "FPSCombatSystem/FPSCombatGameplayTags.h"
#include "Components/ActorComponent.h"
#include "FPSCombatBaseComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPercentChanged, float, Percent);


UCLASS( Abstract )
class FPSCOMBATSYSTEM_API UFPSCombatBaseComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UPROPERTY(BlueprintAssignable)
	FOnPercentChanged OnPercentChanged;
	
	virtual void InitializeWithAbilitySystem(UFPSCombatAbilitySystemComponent* ASC);
	virtual void UninitializeFromAbilitySystem();

	virtual float GetMerged() PURE_VIRTUAL(UFPSCombatBaseComponent::GetMerged, return 0.0f;);
	
protected:
	UPROPERTY()
	TObjectPtr<UFPSCombatAbilitySystemComponent> AbilitySystem;

	UPROPERTY()
	TObjectPtr<const UFPSCombatAttributeSet> AttributeSet;


	virtual void BindAttributeDelegate() PURE_VIRTUAL(UFPSCombatBaseComponent::BindAttributeDelegate, );
	virtual void UnBindAttributeDelegate() PURE_VIRTUAL(UFPSCombatBaseComponent::UnBindAttributeDelegate, );
	
	
};
