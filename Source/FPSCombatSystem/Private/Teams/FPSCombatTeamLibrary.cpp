// Fill out your copyright notice in the Description page of Project Settings.


#include "Teams/FPSCombatTeamLibrary.h"

#include "GenericTeamAgentInterface.h"

bool UFPSCombatTeamLibrary::IsHostileTo(const UObject* A, const UObject* B)
{
	if (!A || !B) return false;

	const IGenericTeamAgentInterface* AAgent = Cast<IGenericTeamAgentInterface>(A);
	if (!AAgent) return false;
	int32 ATeamID = AAgent->GetGenericTeamId();

	const IGenericTeamAgentInterface* BAgent = Cast<IGenericTeamAgentInterface>(B);
	if (!BAgent) return false;
	int32 BTeamID = BAgent->GetGenericTeamId();

	return FGenericTeamId::GetAttitude(ATeamID, BTeamID) == ETeamAttitude::Hostile;
}
