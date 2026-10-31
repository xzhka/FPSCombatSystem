// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Components/GameStateComponent.h"
#include "FPSCombatAISpawnerComponent.generated.h"

/**
 * 
 */
UCLASS()
class FPSCOMBATSYSTEM_API UFPSCombatAISpawnerComponent : public UGameStateComponent
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Gameplay)
	int32 DesiredBotsToCreate = 3;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Gameplay)
	TSubclassOf<AAIController> AIClass;

	UFUNCTION(BlueprintCallable, Category = Spawn)
	void SpawnServerAI();	

	UFUNCTION(BlueprintCallable, Category = Spawn)
	void SpawnOneAIBot();

	UFUNCTION(BlueprintCallable, Category = Spawn)
	void RemoveOneAIBot();
	
private:
	UPROPERTY(Transient)
	TArray<TObjectPtr<AAIController>> SpawnedBotList;
	
};
