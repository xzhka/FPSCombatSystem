// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/FPSCombatMovementComp.h"

#include "Components/CapsuleComponent.h"
#include "FPSCombatSystem/FPSCombatGameplayTags.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameModes/FPSCombatPlayerState.h"

namespace FPSCombatTraceDistance
{
	const float GroundTraceDistance = 10000.0f;
}

// Sets default values for this component's properties
UFPSCombatMovementComp::UFPSCombatMovementComp()
{
	OwnerCharacter = nullptr;
}

const FPSCombatGroundInfo& UFPSCombatMovementComp::GetGroundInfo()
{
	if (!OwnerCharacter || (GFrameCounter == CachedGroundInfo.LastUpdateFrame))
	{
		return CachedGroundInfo;
	}
	
	UCharacterMovementComponent* MoveComp = OwnerCharacter->GetCharacterMovement();
	
	if (MoveComp->MovementMode == MOVE_Walking)
	{
		CachedGroundInfo.GroundInfoHitResult = MoveComp->CurrentFloor.HitResult;
		CachedGroundInfo.GroundDistance = 0.f;
	}
	else
	{
		const UCapsuleComponent* Capsule = OwnerCharacter->GetCapsuleComponent();
		check(Capsule);

		const float HalfCapsuleHeight = Capsule->GetUnscaledCapsuleHalfHeight();
	
		const FVector TraceStart(MoveComp->GetActorLocation());
		const FVector TraceEnd(TraceStart.X, TraceStart.Y, (TraceStart.Z-FPSCombatTraceDistance::GroundTraceDistance - HalfCapsuleHeight));
		const ECollisionChannel TraceChannel = (MoveComp->UpdatedComponent ? MoveComp->UpdatedComponent->GetCollisionObjectType() : ECC_Pawn);

		FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(FPSCombat_GroundInfo), false, OwnerCharacter);
		FCollisionResponseParams ResponseParams;

		MoveComp->InitCollisionParams(QueryParams, ResponseParams);
		
		FHitResult HitResult;
		GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, TraceChannel, QueryParams, ResponseParams);

		CachedGroundInfo.GroundInfoHitResult = HitResult;
		CachedGroundInfo.GroundDistance = FPSCombatTraceDistance::GroundTraceDistance;

		if (MoveComp->MovementMode == MOVE_NavWalking)
		{
			CachedGroundInfo.GroundDistance = 0.f;
		}
		else if (HitResult.bBlockingHit)
		{
			CachedGroundInfo.GroundDistance = FMath::Max((HitResult.Distance - HalfCapsuleHeight), 0.f);
		}
	}
	CachedGroundInfo.LastUpdateFrame = GFrameCounter;
	
	return CachedGroundInfo;
}

float UFPSCombatMovementComp::GetMoveSpeedMultiplier() const
{
	if (CachedASC)
	{
		CachedASC->GetNumericAttribute(UFPSCombatAttributeSet::GetMoveSpeedAttribute());
	}
	return 1.f;
}

void UFPSCombatMovementComp::BeginPlay()
{
	Super::BeginPlay();
	
	OwnerCharacter = Cast<AFPSCombatCharacter>(GetOwner());
}

void UFPSCombatMovementComp::HandleMoveSpeedMultiplierChanged(const FOnAttributeChangeData& Data)
{
	if (OwnerCharacter)
	{
		OwnerCharacter->GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed * Data.NewValue;
	}
}

void UFPSCombatMovementComp::HandleMovementUpdated(float DeltaSeconds, FVector OldLocation, FVector OldVelocity)
{
	if (!CachedASC || !OwnerCharacter) return;
	
	const float MoveSpeed = OwnerCharacter->GetCharacterMovement()->Velocity.SizeSquared2D();
	const bool bNowMoving = MoveSpeed > FMath::Square(Threshold);
	
	if (bNowMoving != bIsWalking)
	{
		bIsWalking = bNowMoving;
		CachedASC->SetLooseGameplayTagCount(FPSCombatGameplayTags::State_Moving_Walking, bIsWalking ? 1 : 0);
	}

	const FVector CurrentAcceleration = OwnerCharacter->GetCharacterMovement()->GetCurrentAcceleration();
	if (!CurrentAcceleration.IsNearlyZero())
	{
		const float ForwardDot = FVector::DotProduct(CurrentAcceleration.GetSafeNormal(),OwnerCharacter->GetActorForwardVector());
		const bool bNowForward = ForwardDot > ForwardDotThreshold;
		if (bNowForward != bIsWalkingForward)
		{
			bIsWalkingForward = bNowForward;
			CachedASC->SetLooseGameplayTagCount(FPSCombatGameplayTags::State_Moving_MovingForward, bIsWalkingForward ? 1 : 0);
		}
	}
}

void UFPSCombatMovementComp::HandleMovementModeChanged(ACharacter* Character, EMovementMode PrevMovementMode,
	uint8 PreviousCustomMode)
{
	if (!OwnerCharacter) return;
	
	const EMovementMode NewMode = OwnerCharacter->GetCharacterMovement()->MovementMode;
	const bool bIsGrounded = (NewMode == MOVE_Walking || NewMode == MOVE_NavWalking);
	
	SetMovementState(bIsGrounded ? EFPSCombatMoveState::Grounded : EFPSCombatMoveState::Airborne);
}

void UFPSCombatMovementComp::SetMovementState(EFPSCombatMoveState NewState)
{
	if (NewState == CurrentMoveState || !CachedASC) return;
	CurrentMoveState = NewState;

	const bool bAirborne = (NewState == EFPSCombatMoveState::Airborne);
	CachedASC->SetLooseGameplayTagCount(FPSCombatGameplayTags::State_Moving_Airborne, bAirborne ? 1 : 0);
	if (!bAirborne && OwnerCharacter->HasAuthority())
	{
		CachedASC->RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(FPSCombatGameplayTags::Ability_Moving_AirborneSource));
	}
}

FVector UFPSCombatMovementComp::GetDashDirection() const
{
	if (!OwnerCharacter) return FVector::ZeroVector;

	UCharacterMovementComponent* MoveComp = OwnerCharacter->GetCharacterMovement();
	
	FVector Direction = MoveComp->GetCurrentAcceleration().GetSafeNormal();

	if (Direction.IsNearlyZero())
	{
		Direction = OwnerCharacter->GetActorForwardVector();
	}

	return Direction;
}


void UFPSCombatMovementComp::InitializeWithAbilitySystem(UFPSCombatAbilitySystemComponent* ASC)
{
	if (!ASC || CachedASC==ASC) { return; }

	CachedASC = ASC;

	if (!OwnerCharacter)
	{
		OwnerCharacter = Cast<AFPSCombatCharacter>(GetOwner());
	}
	
	ASC->GetGameplayAttributeValueChangeDelegate(UFPSCombatAttributeSet::GetMoveSpeedAttribute())
				.AddUObject(this, &UFPSCombatMovementComp::HandleMoveSpeedMultiplierChanged);
	
	
	if (OwnerCharacter)
	{
		OwnerCharacter->OnCharacterMovementUpdated.AddDynamic(this, &UFPSCombatMovementComp::HandleMovementUpdated);

		OwnerCharacter->MovementModeChangedDelegate.AddUniqueDynamic(this, &UFPSCombatMovementComp::HandleMovementModeChanged);
	}
}

void UFPSCombatMovementComp::Dash(const float Strength, const float Duration)
{
	if (!OwnerCharacter) return;
	
	UCharacterMovementComponent* CharacterMoveComp = OwnerCharacter->GetCharacterMovement();
	
	TSharedPtr<FRootMotionSource_ConstantForce> ConstantForce = MakeShared<FRootMotionSource_ConstantForce>();

	ConstantForce->InstanceName = TEXT("DashConstantForce");
	ConstantForce->Force = GetDashDirection() * Strength;
	ConstantForce->Duration = Duration;
	ConstantForce->Priority = 5;   
	ConstantForce->FinishVelocityParams.ClampVelocity = BaseWalkSpeed;
	ConstantForce->AccumulateMode = ERootMotionAccumulateMode::Additive;

	
	ConstantForce->FinishVelocityParams.Mode = ERootMotionFinishVelocityMode::ClampVelocity;


	uint16 ApplySourceID = CharacterMoveComp->ApplyRootMotionSource(ConstantForce);
	
}

void UFPSCombatMovementComp::Updraft(float Distance)
{
	if (!OwnerCharacter) return;

	 const FVector LaunchVelocity(0.f,0.f,Distance);
	
	OwnerCharacter->LaunchCharacter(LaunchVelocity, false, true);
}
