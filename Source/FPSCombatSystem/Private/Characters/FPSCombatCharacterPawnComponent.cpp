// FPS Combat project


#include "Characters/FPSCombatCharacterPawnComponent.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Characters/FPSCombatCharacter.h"
#include "GameModes/FPSCombatPlayerState.h"

void UFPSCombatCharacterPawnComponent::InitializeInputComponents(UInputComponent* PlayerInputComponent)
{
	if (APlayerController* PC = Cast<APlayerController>(GetController<APlayerController>()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* LocalPlayerSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			if (InputConfig && InputConfig->DefaultMappingContext)
			{
				LocalPlayerSubsystem->AddMappingContext(InputConfig->DefaultMappingContext,0);
			}
		}
	}

	// Move, look and jump are bound directly. Every other action is bound
	// generically and routed to the ASC by its InputTag.
	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &UFPSCombatCharacterPawnComponent::Move);
		EIC->BindAction(LookAction, ETriggerEvent::Triggered, this, &UFPSCombatCharacterPawnComponent::Look);

		EIC->BindAction(JumpAction, ETriggerEvent::Started, this, &UFPSCombatCharacterPawnComponent::Jump);
		EIC->BindAction(JumpAction, ETriggerEvent::Completed, this, &UFPSCombatCharacterPawnComponent::StopJump);

		if (!InputConfig->InputActions.IsEmpty())
		{
			for (const FPSInputAction& Action : InputConfig->InputActions)
			{
				if (Action.InputTag.IsValid() && Action.BaseInputActions)
				{
					EIC->BindAction(Action.BaseInputActions, ETriggerEvent::Started, this, &UFPSCombatCharacterPawnComponent::Ability_InputTagPressed, Action.InputTag);

					EIC->BindAction(Action.BaseInputActions, ETriggerEvent::Completed, this, &UFPSCombatCharacterPawnComponent::Ability_InputTagReleased, Action.InputTag);
				}
			}
		}
	}
}

void UFPSCombatCharacterPawnComponent::Move(const FInputActionValue& Value)
{
	
	FVector2D Location = Value.Get<FVector2D>();
	if (APawn* Pawn = GetPawn<APawn>())
	{
		if (Pawn->Controller)
		{
			const FRotator MovementRotation(0.0f, Pawn->GetControlRotation().Yaw, 0.0f);
			const FVector ForwardVector = MovementRotation.RotateVector(FVector::ForwardVector); 
			const FVector RightVector = MovementRotation.RotateVector(FVector::RightVector); 
		
			Pawn->AddMovementInput(ForwardVector, Location.Y);
			Pawn->AddMovementInput(RightVector, Location.X);
		}
	}
}

void UFPSCombatCharacterPawnComponent::Look(const FInputActionValue& Value)
{
	const FVector2D Location = Value.Get<FVector2D>();

	if (APawn* Pawn = GetPawn<APawn>())
	{
		Pawn->AddControllerYawInput(Location.X);
		Pawn->AddControllerPitchInput(Location.Y);
	}
}

void UFPSCombatCharacterPawnComponent::Ability_InputTagPressed(FGameplayTag InputTag)
{
	APawn* Pawn = GetPawn<APawn>();
	if (AFPSCombatPlayerState* PlayerState = Pawn ? Pawn->GetPlayerState<AFPSCombatPlayerState>() : nullptr)
	{
		if (UFPSCombatAbilitySystemComponent* ASC = Cast<UFPSCombatAbilitySystemComponent>(PlayerState->GetAbilitySystemComponent()))
		{
			ASC->AbilityInputTagPressed(InputTag);
		}
	}
	
}

void UFPSCombatCharacterPawnComponent::Ability_InputTagReleased(FGameplayTag InputTag)
{
	APawn* Pawn = GetPawn<APawn>();
	if (AFPSCombatPlayerState* PlayerState = Pawn ? Pawn->GetPlayerState<AFPSCombatPlayerState>() : nullptr)
	{
		if (UFPSCombatAbilitySystemComponent* ASC = Cast<UFPSCombatAbilitySystemComponent>(PlayerState->GetAbilitySystemComponent()))
		{
			ASC->AbilityInputTagReleased(InputTag);
		}
	}
}


void UFPSCombatCharacterPawnComponent::Jump()
{
	if (ACharacter* Character = GetPawn<ACharacter>())
	{
		Character->Jump();
	}
}

void UFPSCombatCharacterPawnComponent::StopJump()
{
	if (ACharacter* Character = GetPawn<ACharacter>())
	{
		Character->StopJumping();
	}
}
