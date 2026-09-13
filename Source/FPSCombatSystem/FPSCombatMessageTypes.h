#pragma once

#include "CoreMinimal.h"
#include "Items/FPSCombatItemInstance.h"
#include "FPSCombatMessageTypes.generated.h"

class UFPSCombatEquipmentDefinition;
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
	TSubclassOf<UFPSCombatItemInstance> ItemDefinition = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Items")
	FGameplayTag ItemTag;
	
	UPROPERTY(BlueprintReadOnly, Category = "Items")
	int32 NewCount = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Items")
	int32 DeltaCount = 0;
};

USTRUCT(BlueprintType)
struct FFPSCombatSlotsChangedMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Slots")
	TObjectPtr<AActor> OwnerActor = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Slots")
	TArray<TObjectPtr<UFPSCombatItemInstance>> ItemSlots;
};

USTRUCT(BlueprintType)
struct FFPSCombatActiveSlotChangedMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Slots")
	TObjectPtr<AActor> OwnerActor = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Slots")
	int32 NewActiveSlot = -1;
};