// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/FPSCombatCharacterPawnComp.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Characters/FPSCombatCharacter.h"
#include "GameModes/FPSCombatPlayerState.h"

void UFPSCombatCharacterPawnComp::InitializeInputComponents(UInputComponent* PlayerInputComponent)
{
	UE_LOG(LogTemp, Warning, TEXT("InitializeInputComponents called, this=%p"), this);
	if (APlayerController* PC = Cast<APlayerController>(GetController<APlayerController>()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* LocalPlayerSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			if (InputConfig && InputConfig->DefaultMappingContext)
			{
				UE_LOG(LogTemp, Warning, TEXT("Mapping Context is added"));
				LocalPlayerSubsystem->AddMappingContext(InputConfig->DefaultMappingContext,0);
			}
		}
	}

	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &UFPSCombatCharacterPawnComp::Move);
		EIC->BindAction(LookAction, ETriggerEvent::Triggered, this, &UFPSCombatCharacterPawnComp::Look);

		EIC->BindAction(JumpAction, ETriggerEvent::Started, this, &UFPSCombatCharacterPawnComp::Jump);
		EIC->BindAction(JumpAction, ETriggerEvent::Completed, this, &UFPSCombatCharacterPawnComp::StopJump);

		for (const FPSInputAction& Action : InputConfig->InputActions)
		{
			if (Action.InputTag.IsValid() && Action.BaseInputActions)
			{
				EIC->BindAction(Action.BaseInputActions, ETriggerEvent::Started, this, &UFPSCombatCharacterPawnComp::Ability_InputTagPressed, Action.InputTag);

				EIC->BindAction(Action.BaseInputActions, ETriggerEvent::Completed, this, &UFPSCombatCharacterPawnComp::Ability_InputTagReleased, Action.InputTag);
			}
		}
	}
}

void UFPSCombatCharacterPawnComp::Move(const FInputActionValue& Value)
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

void UFPSCombatCharacterPawnComp::Look(const FInputActionValue& Value)
{
	const FVector2D Location = Value.Get<FVector2D>();

	if (APawn* Pawn = GetPawn<APawn>())
	{
		Pawn->AddControllerYawInput(Location.X);
		Pawn->AddControllerPitchInput(Location.Y);
	}
}

void UFPSCombatCharacterPawnComp::Ability_InputTagPressed(FGameplayTag InputTag)
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

void UFPSCombatCharacterPawnComp::Ability_InputTagReleased(FGameplayTag InputTag)
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


void UFPSCombatCharacterPawnComp::Jump()
{
	if (ACharacter* Character = GetPawn<ACharacter>())
	{
		Character->Jump();
	}
}

void UFPSCombatCharacterPawnComp::StopJump()
{
	if (ACharacter* Character = GetPawn<ACharacter>())
	{
		Character->StopJumping();
	}
}
