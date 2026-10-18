// FPS Combat project


#include "FPSCombatSystem/Public/GameModes/FPSCombatPlayerState.h"

#include "FPSCombatSystem/FPSCombatSystem.h"
#include "GameModes/FPSCombatGameMode.h"
#include "Net/UnrealNetwork.h"

AFPSCombatPlayerState::AFPSCombatPlayerState()
{
	ASC = CreateDefaultSubobject<UFPSCombatAbilitySystemComponent>(TEXT("AbilitySystemComponent"));

	ASC->SetIsReplicated(true);
	SetNetUpdateFrequency(100.f);
	ASC->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	
	AttributeSet = CreateDefaultSubobject<UFPSCombatAttributeSet>(TEXT("AttributeSet"));

	ItemComponent = CreateDefaultSubobject<UFPSCombatItemManagerComponent>(TEXT("ItemManager"));
}

void AFPSCombatPlayerState::SetPawnData(const UFPSCombatPawnData* InPawnData)
{
	if (GetLocalRole() != ROLE_Authority || PawnData)
	{
		return;
	}
	PawnData = InPawnData;
	
	for (UFPSCombatAbilitySet* AbilitySet : PawnData->AbilitySets)
	{
		if (AbilitySet)
		{
			AbilitySet->GiveAbility(ASC, nullptr);
		}
	}
	ForceNetUpdate();
}

void AFPSCombatPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (HasAuthority())
	{
		if (AFPSCombatGameMode* GameMode = GetWorld()->GetAuthGameMode<AFPSCombatGameMode>())
		{
			SetPawnData(GameMode->GetPlayerPawnData(GetOwningController()));
		}
	}
}

UAbilitySystemComponent* AFPSCombatPlayerState::GetAbilitySystemComponent() const
{
	return ASC;
}

UAttributeSet* AFPSCombatPlayerState::GetAttributeSet() const
{
	return AttributeSet;
}

void AFPSCombatPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFPSCombatPlayerState, PawnData);
	
}

void AFPSCombatPlayerState::OnRep_PawnData()
{
}
