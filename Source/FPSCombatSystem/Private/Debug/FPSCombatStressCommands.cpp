
#include "AIController.h"
#include "Characters/FPSCombatCharacter.h"
#include "HAL/IConsoleManager.h"
#include "Engine/World.h"
#include "FPSCombatSystem/FPSCombatSystem.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"

#if !UE_BUILD_SHIPPING

namespace FPSCombatStress
{
	TArray<TWeakObjectPtr<AFPSCombatCharacter>> GStressCharacters;

	FTSTicker::FDelegateHandle GStressTickerHandle;

	FTransform GetRingTransform(const APawn& Pawn, int32 Index, int32 Count)
	{
		const float AngleDeg = Pawn.GetActorRotation().Yaw + Index * (360.f / Count);
		
		const float Distance = 200.f + (Index % 4) * 200.f;

		const FVector Offset = FRotator(0.f, AngleDeg, 0.f).RotateVector(FVector::ForwardVector) * Distance;

		return FTransform(FRotator::ZeroRotator, Pawn.GetActorLocation() + Offset + FVector(0.f, 0.f, 100.f));
	}
	
	void AttachController(UWorld& World, AFPSCombatCharacter& Character)
	{
		if (Character.GetController()) return;

		AAIController* AIController = World.SpawnActorDeferred<AAIController>(AAIController::StaticClass(), FTransform::Identity,
			nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		if (!AIController) return;
		AIController->bWantsPlayerState = true;
		AIController->FinishSpawning(FTransform::Identity);
		AIController->Possess(&Character);

		UE_LOG(LogFPSCombat, Log, TEXT("Stress. Character : %s, PlayerState : %s"), *GetNameSafe(&Character), *GetNameSafe(Character.GetPlayerState()));
	}
	
	AFPSCombatCharacter* SpawnStressCharacter(UWorld& World, UClass* PawnClass, const FTransform& Transform)
	{
		AFPSCombatCharacter* Char = World.SpawnActorDeferred<AFPSCombatCharacter>(
			PawnClass, Transform, nullptr, nullptr,
			ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);

		if (Char)
		{
			Char->FinishSpawning(Transform);
			AttachController(World, *Char);
		}
			
		return Char;
	}

	void StartDriving(UWorld* World)
	{
		if (GStressTickerHandle.IsValid()) return;

		TWeakObjectPtr<UWorld> WeakWorldPtr = World;

		GStressTickerHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([WeakWorldPtr](float DeltaTime)
		{
			UWorld* W = WeakWorldPtr.Get();
			if (!W)
			{
				GStressTickerHandle.Reset();
				return false;
			}

			const float Time = W->GetTimeSeconds();	

			for (int32 i=0; i<GStressCharacters.Num(); i++)
			{
				ACharacter* Char = GStressCharacters[i].Get();
				if (!Char) continue;

				const float Yaw = i * 37.f + FMath::FloorToFloat(Time / 3.f) * 45.f;
				Char->AddMovementInput(FRotator(0.f, Yaw, 0.f).Vector(), 1.f);
				
				if (FMath::Fmod(Time + i * 0.13f, 2.f) < DeltaTime)
				{
					Char->Jump();
				}
			}
			
			return true;
		}));
	}
	
	void StopAll()
	{
		if (GStressTickerHandle.IsValid())
		{
			FTSTicker::GetCoreTicker().RemoveTicker(GStressTickerHandle);
			GStressTickerHandle.Reset();
		}
		
		for (TWeakObjectPtr<AFPSCombatCharacter>& Weak : GStressCharacters)
		{
			if (AFPSCombatCharacter* Char = Weak.Get())
			{
				if (AController* Controller = Char->GetController())
				{
					Controller->Destroy();   // grab it first, the pawn loses it when destroyed
				}
				Char->Destroy();
			}
		}
		GStressCharacters.Reset();
	}

	void SpawnCharacters(const TArray<FString>& Args, UWorld* World)
	{
		if (!World) return;

		APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(World, 0);
		if (!PlayerPawn) return;

		StopAll();
		
		const int32 Count = Args.Num() > 0 ? FCString::Atoi(*Args[0]) : 16;

		for (int32 i = 0; i < Count; ++i)
		{
			if (AFPSCombatCharacter* Char = SpawnStressCharacter(*World, PlayerPawn->GetClass(), GetRingTransform(*PlayerPawn, i, Count)))
			{
				GStressCharacters.Add(Char);
			}
		}
		StartDriving(World);
	}
}

static FAutoConsoleCommandWithWorldAndArgs GStressCharsCmd(TEXT("FPSCombat.StressChars"),
	TEXT("Spawn N characters that run and jump. Usage: FPSCombat.StressChars <N>"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateStatic(&FPSCombatStress::SpawnCharacters));

static FAutoConsoleCommand GStressStopCmd(
	TEXT("FPSCombat.StressStop"),
	TEXT("Destroy all stress characters."),
	FConsoleCommandDelegate::CreateStatic(&FPSCombatStress::StopAll));

#endif // !UE_BUILD_SHIPPING