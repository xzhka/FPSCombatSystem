// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AbilitySystem/FPSCombatAbilitySystemComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Perception/AISenseConfig_Sight.h"
#include "FPSCombatEnemyController.generated.h"

/**
 * 
 */
UCLASS()
class FPSCOMBATSYSTEM_API AFPSCombatEnemyController : public AAIController
{
	GENERATED_BODY()

public:

	AFPSCombatEnemyController();
	
	virtual FGenericTeamId GetGenericTeamId() const override;
	virtual void PostInitializeComponents() override;
protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

	UFUNCTION()
	void OnDeathStarted(AActor* OwningActor);

	UFUNCTION()
	void OnTargetPerceptionUpdate(AActor* Actor, FAIStimulus Stimulus);
	
	UFPSCombatAbilitySystemComponent* GetCombatASC() const;
	

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = AI)
	UAISenseConfig_Sight* Sight;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Behavior Tree")
	TObjectPtr<UBehaviorTree> BehaviorTree;
	
};
