// FPS Combat project


#include "Weapons/Projectile/FPSCombatThrowableInstance.h"

#include "Engine/AssetManager.h"
#include "FPSCombatSystem/FPSCombatGameplayTags.h"

UFPSCombatThrowableDefinition* UFPSCombatThrowableInstance::GetThrowableDefinition() const
{
	if (GetDefinition())
	{
		return Cast<UFPSCombatThrowableDefinition>(GetDefinition());
	}
	return nullptr;
}

bool UFPSCombatThrowableInstance::HasChargesRemaining() const
{
	UFPSCombatItemInstance* ItemInstance = GetItemInstance();
	return ItemInstance && (ItemInstance->GetStack(FPSCombatGameplayTags::Data_Projectile_Quantity) > 0);
}

void UFPSCombatThrowableInstance::ConsumeProjectile(FGameplayTag ProjectileTag) const
{
	UFPSCombatItemInstance* ItemInstance = GetItemInstance();

	ItemInstance->RemoveStackCount(ProjectileTag, 1);
}

UAnimMontage* UFPSCombatThrowableInstance::GetAnimDefinitionMontage(FGameplayTag ActionTag) const
{
	const TObjectPtr<UAnimMontage>* ThrowAnimation = CachedActionMontages.Find(ActionTag);
	return ThrowAnimation ? *ThrowAnimation : nullptr;
}

void UFPSCombatThrowableInstance::OnEquipped()
{
	Super::OnEquipped();

	if (UFPSCombatThrowableDefinition* ThrowableDef = GetThrowableDefinition())
	{
		TArray<FSoftObjectPath> SoftObjectPaths;
		for (const auto& Pair : ThrowableDef->ActionAnimations)
		{
			if (!Pair.Value.IsNull()) SoftObjectPaths.Add(Pair.Value.ToSoftObjectPath());
		}
		if (!ThrowableDef->ProjectileClass.IsNull())
		{
			SoftObjectPaths.AddUnique(ThrowableDef->ProjectileClass.ToSoftObjectPath());
		}
		
		if (SoftObjectPaths.Num() > 0)
		{
			ProjectileHandle = UAssetManager::GetStreamableManager().RequestAsyncLoad(SoftObjectPaths, FStreamableDelegate::CreateUObject(this, &UFPSCombatThrowableInstance::OnThrowAssetsLoaded));
		}
	}
}

void UFPSCombatThrowableInstance::OnUnequipped()
{
	if (ProjectileHandle.IsValid())
	{
		ProjectileHandle->CancelHandle();
		ProjectileHandle.Reset();
	}
	CachedActionMontages.Empty();
	CachedProjectileClass = nullptr;
	
	Super::OnUnequipped();
}

void UFPSCombatThrowableInstance::OnThrowAssetsLoaded()
{
	if (UFPSCombatThrowableDefinition* ThrowDef = GetThrowableDefinition())
	{
		CachedActionMontages.Empty();
		for (const auto& Pair : ThrowDef->ActionAnimations)
		{
			if (UAnimMontage* Loaded = Pair.Value.Get())
			{
				CachedActionMontages.Add(Pair.Key, Loaded);
			}
		}

		CachedProjectileClass = ThrowDef->ProjectileClass.Get();
	}
}
