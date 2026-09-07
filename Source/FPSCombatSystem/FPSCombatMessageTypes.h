#pragma once

#include "CoreMinimal.h"
#include "Items/FPSCombatItemInstance.h"
#include "FPSCombatMessageTypes.generated.h"


class UFPSCombatEquipmentInstance;

USTRUCT(BlueprintType)
struct FFPSCombatAmmoChangedMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	int32 CurrentAmmo = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 ReserveAmmo = 0;
};

USTRUCT(BlueprintType)
struct FFPSCombatEquipmentChangedMessage
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UFPSCombatEquipmentInstance> NewObjectInstance = nullptr;

	UPROPERTY(BlueprintReadOnly)
	bool bIsEquipped = false;
	
};

USTRUCT(BlueprintType)
struct FFPSCombatItemChangedMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UFPSCombatItemInstance> ItemInstance = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Items")
	TSubclassOf<UFPSCombatEquipmentDefinition> ItemDefinition = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Items")
	FGameplayTag ItemTag;
	
	UPROPERTY(BlueprintReadOnly, Category = "Items")
	int32 NewCount = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Items")
	int32 DeltaCount = 0;
	
};
