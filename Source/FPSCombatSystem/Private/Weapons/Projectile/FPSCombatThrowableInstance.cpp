// FPS Combat project


#include "Weapons/Projectile/FPSCombatThrowableInstance.h"

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