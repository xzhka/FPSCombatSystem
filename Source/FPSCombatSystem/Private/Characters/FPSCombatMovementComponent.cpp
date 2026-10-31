// FPS Combat project


#include "Characters/FPSCombatMovementComponent.h"

#include "Components/CapsuleComponent.h"
#include "FPSCombatSystem/FPSCombatGameplayTags.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameModes/FPSCombatPlayerState.h"

namespace FPSCombatTraceDistance
{
	const float GroundTraceDistance = 10000.0f;
}

UFPSCombatMovementComponent::UFPSCombatMovementComponent()
{
	OwnerCharacter = nullptr;
}

const FPSCombatGroundInfo& UFPSCombatMovementComponent::GetGroundInfo()
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
		TRACE_CPUPROFILER_EVENT_SCOPE(FPSCombat_GroundTrace);
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

		// Nav-walking has no meaningful gap, so report 0. Otherwise, measure
		// from the capsule bottom.
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

float UFPSCombatMovementComponent::GetMoveSpeedMultiplier() const
{
	if (CachedASC)
	{
		return CachedASC->GetNumericAttribute(UFPSCombatAttributeSet::GetMoveSpeedAttribute());
	}
	return 1.f;
}

void UFPSCombatMovementComponent::BeginPlay()
{
	Super::BeginPlay();
	
	OwnerCharacter = Cast<AFPSCombatCharacter>(GetOwner());
}

void UFPSCombatMovementComponent::HandleMoveSpeedMultiplierChanged(const FOnAttributeChangeData& Data)
{
	if (OwnerCharacter)
	{
		OwnerCharacter->GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed * Data.NewValue;
	}
}

void UFPSCombatMovementComponent::HandleMovementUpdated(float DeltaSeconds, FVector OldLocation, FVector OldVelocity)
{
	if (!CachedASC || !OwnerCharacter) return;
	
	const float MoveSpeed = OwnerCharacter->GetCharacterMovement()->Velocity.SizeSquared2D();
	const bool bNowMoving = MoveSpeed > FMath::Square(Threshold);

	// Mirror movement into gameplay tag, so another code can
	// react on it.
	if (bNowMoving != bIsWalking)
	{
		bIsWalking = bNowMoving;
		CachedASC->SetLooseGameplayTagCount(FPSCombatGameplayTags::State_Moving_Walking, bIsWalking ? 1 : 0);
	}

	// Forward means input acceleration roughly aligned with the facing
	// direction. With no input the previous value is kept.
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

void UFPSCombatMovementComponent::HandleMovementModeChanged(ACharacter* Character, EMovementMode PrevMovementMode,
	uint8 PreviousCustomMode)
{
	if (!OwnerCharacter) return;

	if (OwnerCharacter->GetCharacterMovement()->MovementMode == MOVE_None) return;

	// Determine the move state.
	const EMovementMode NewMode = OwnerCharacter->GetCharacterMovement()->MovementMode;
	const bool bIsGrounded = (NewMode == MOVE_Walking || NewMode == MOVE_NavWalking);
	
	SetMovementState(bIsGrounded ? EFPSCombatMoveState::Grounded : EFPSCombatMoveState::Airborne);
}

void UFPSCombatMovementComponent::SetMovementState(EFPSCombatMoveState NewState)
{
	if (NewState == CurrentMoveState || !CachedASC) return;
	CurrentMoveState = NewState;

	const bool bAirborne = (NewState == EFPSCombatMoveState::Airborne);
	CachedASC->SetLooseGameplayTagCount(FPSCombatGameplayTags::State_Moving_Airborne, bAirborne ? 1 : 0);

	// On landing, authority owner remove effect which grants the airborne-source tag.
	if (!bAirborne && OwnerCharacter->HasAuthority())
	{
		CachedASC->RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(FPSCombatGameplayTags::Ability_Moving_AirborneSource));
	}
}

void UFPSCombatMovementComponent::ClearASCGameplayTags()
{
	if (CachedASC)
	{
		CachedASC->SetLooseGameplayTagCount(FPSCombatGameplayTags::State_Moving_Walking, 0);
		CachedASC->SetLooseGameplayTagCount(FPSCombatGameplayTags::State_Moving_Airborne, 0);
		CachedASC->SetLooseGameplayTagCount(FPSCombatGameplayTags::State_Moving_MovingForward, 0);
	}
}

FVector UFPSCombatMovementComponent::GetDashDirection() const
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


void UFPSCombatMovementComponent::InitializeWithAbilitySystem(UFPSCombatAbilitySystemComponent* ASC)
{
	if (!ASC || CachedASC==ASC) { return; }

	CachedASC = ASC;

	if (!OwnerCharacter)
	{
		OwnerCharacter = Cast<AFPSCombatCharacter>(GetOwner());
	}

	// Follow MoveSpeed changes, and mirrors movement updates and
	// mode changes into gameplay tags.
	ASC->GetGameplayAttributeValueChangeDelegate(UFPSCombatAttributeSet::GetMoveSpeedAttribute())
				.AddUObject(this, &UFPSCombatMovementComponent::HandleMoveSpeedMultiplierChanged);
	
	
	if (OwnerCharacter)
	{
		OwnerCharacter->OnCharacterMovementUpdated.AddDynamic(this, &UFPSCombatMovementComponent::HandleMovementUpdated);

		OwnerCharacter->MovementModeChangedDelegate.AddUniqueDynamic(this, &UFPSCombatMovementComponent::HandleMovementModeChanged);
	}
}

void UFPSCombatMovementComponent::UninitializeFromAbilitySystem()
{
	if (CachedASC)
	{
		ClearASCGameplayTags();
		CachedASC->GetGameplayAttributeValueChangeDelegate(
			UFPSCombatAttributeSet::GetMoveSpeedAttribute()).RemoveAll(this);
	}
	
	CachedASC = nullptr;

	if (OwnerCharacter)
	{
		OwnerCharacter->OnCharacterMovementUpdated.RemoveAll(this);
		OwnerCharacter->MovementModeChangedDelegate.RemoveAll(this);
	}
}

void UFPSCombatMovementComponent::Dash(const float Strength, const float Duration)
{
	if (!OwnerCharacter) return;
	
	UCharacterMovementComponent* CharacterMoveComp = OwnerCharacter->GetCharacterMovement();

	// A short constant-force burst. 
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

void UFPSCombatMovementComponent::Updraft(float Distance)
{
	if (!OwnerCharacter) return;

	 const FVector LaunchVelocity(0.f,0.f,Distance);
	
	OwnerCharacter->LaunchCharacter(LaunchVelocity, false, true);
}
