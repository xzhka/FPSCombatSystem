// FPS Combat project

#pragma once

#include "CoreMinimal.h"
#include "FPSCombatEquipmentManager.h"
#include "NativeGameplayTags.h"
#include "Components/ControllerComponent.h"
#include "Items/FPSCombatItemInstance.h"
#include "FPSCombatQuickBarComponent.generated.h"



/** UFPSCombatQuickBarComponent
 *
 *	Controller component representing
 *	the quick bar, working with items
 *	adding and removing
 */
UCLASS()
class FPSCOMBATSYSTEM_API UFPSCombatQuickBarComponent : public UControllerComponent
{
	GENERATED_BODY()

	UFPSCombatQuickBarComponent(const FObjectInitializer& ObjectInitializer);

public:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category="Equipment")
	void CycleSlotForward();

	UFUNCTION(BlueprintCallable, Category="Equipment")
	void CycleSlotBackward();
	
	UFUNCTION(BlueprintCallable, BlueprintPure = false)
	FORCEINLINE TArray<UFPSCombatItemInstance*> GetSlots() const { return ItemSlots; }

	UFUNCTION(BlueprintCallable, BlueprintPure = false)
	int32 GetActiveSlotIndex() const { return ActiveSlot; }
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE UFPSCombatEquipmentInstance* GetActiveItemInstance() const { return EquippedItem; }
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Slots")
	void SetActiveSlot(int32 NewIndex);

	UFUNCTION(BlueprintCallable, BlueprintPure = false , Category="Slots")
	UFPSCombatItemInstance* GetActiveSlotItem() const;
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Slots")
	void AddItemToSlot(UFPSCombatItemInstance* Item, int32 ItemSlot);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Slots")
	UFPSCombatItemInstance* RemoveItemFromSlot(int32 ItemSlot);

	int32 GetNextFreeItemSlot();
	
protected:
	void EquipItemInSlot();
	
	void UnequipItemInSlot();
	
	UFUNCTION()
	void OnRep_ItemSlots();

	UFUNCTION()
	void OnRep_ActiveSlot();

	UPROPERTY(EditDefaultsOnly)
	int32 SlotsAmount = 3;
private:
	UFPSCombatEquipmentManager* FindEquipmentManager() const;
	
	UPROPERTY(ReplicatedUsing = OnRep_ItemSlots)
	TArray<TObjectPtr<UFPSCombatItemInstance>> ItemSlots;

	UPROPERTY(ReplicatedUsing = OnRep_ActiveSlot)
	int32 ActiveSlot = -1;
	
	UPROPERTY()
	TObjectPtr<UFPSCombatEquipmentInstance> EquippedItem;
};
