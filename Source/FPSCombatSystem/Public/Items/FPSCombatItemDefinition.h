// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FPSCombatItemInstance.h"
#include "Equipment/FPSCombatEquipmentDefinition.h"
#include "UObject/NoExportTypes.h"
#include "FPSCombatItemDefinition.generated.h"

UCLASS(MinimalAPI, Abstract, EditInlineNew, Blueprintable)
class UFPSCombatItemFragment : public UObject
{
	GENERATED_BODY()

public:
	virtual void OnInstanceCreated(UFPSCombatItemInstance* InItemInstance) const {}
};

UCLASS()
class UFPSCombatItemFragment_Equippable : public UFPSCombatItemFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Definition")
	TSubclassOf<UFPSCombatEquipmentDefinition> EquipmentDefinition;
	
};

UCLASS()
class UFPSCombatItemFragment_QuickBarSlot : public UFPSCombatItemFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "QuickBar")
	TSoftObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "QuickBar")
	FText DisplayName;
};

UCLASS()
class UFPSCombatItemFragment_Stats : public UFPSCombatItemFragment
{
	GENERATED_BODY()

public:
	virtual void OnInstanceCreated(UFPSCombatItemInstance* InItemDefinition) const override;

	int32 GetStatsByTag(FGameplayTag InTag) const;
	
	UPROPERTY(EditAnywhere, Category = "Stats")
	TMap<FGameplayTag, int32> Stats;
};


UCLASS(Blueprintable, BlueprintType)
class FPSCOMBATSYSTEM_API UFPSCombatItemDefinition : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Definition")
	FName DisplayName;
	
	UPROPERTY(EditDefaultsOnly, Instanced, Category = "Fragments")
	TArray<TObjectPtr<UFPSCombatItemFragment>> Fragments;

	const UFPSCombatItemFragment* FindFragmentByClass(TSubclassOf<UFPSCombatItemFragment> FragmentClass) const;

	int32 GetDefaultStatsValueByTag(FGameplayTag InTag) const;
};
