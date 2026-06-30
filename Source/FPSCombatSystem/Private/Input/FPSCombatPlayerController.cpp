// Fill out your copyright notice in the Description page of Project Settings.


#include "Input/FPSCombatPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Characters/FPSCombatCharacter.h"
#include "GameFramework/Character.h"


void AFPSCombatPlayerController::AcknowledgePossession(APawn* P)
{
	Super::AcknowledgePossession(P);

	if (AFPSCombatCharacter* ACharacter = Cast<AFPSCombatCharacter>(P))
	{
		ACharacter->InitializeAbilitySystem();
	}
}

void AFPSCombatPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent))
	{
		EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AFPSCombatPlayerController::Move);
		EIC->BindAction(LookAction, ETriggerEvent::Triggered, this, &AFPSCombatPlayerController::Look);

		EIC->BindAction(JumpAction, ETriggerEvent::Started, this, &AFPSCombatPlayerController::Jump);
		EIC->BindAction(JumpAction, ETriggerEvent::Completed, this, &AFPSCombatPlayerController::StopJump);
	}
}

void AFPSCombatPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	if (InputConfig)
	{
		if (UEnhancedInputLocalPlayerSubsystem* LocalPlayerSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			UE_LOG(LogTemp, Warning, TEXT("Mapping Context is added"));
			LocalPlayerSubsystem->AddMappingContext(InputConfig->DefaultMappingContext,0);
		}
	}

	if (HealthWidgetClass)
	{
		HealthWidget = CreateWidget<UFPSCombatHeathBarWidget>(this, HealthWidgetClass);
		HealthWidget->AddToViewport();
	}
}

void AFPSCombatPlayerController::Move(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("Move triggered"));
	
	FVector2D Location = Value.Get<FVector2D>();

	if (APawn* CurrentPawn = GetPawn())
	{
		const FRotator MovementRotation(0.0f, GetControlRotation().Yaw, 0.0f);
		const FVector ForwardVector = MovementRotation.RotateVector(FVector::ForwardVector); 
		const FVector RightVector = MovementRotation.RotateVector(FVector::RightVector); 
		
		CurrentPawn->AddMovementInput(ForwardVector, Location.Y);
		CurrentPawn->AddMovementInput(RightVector, Location.X);
	}
}

void AFPSCombatPlayerController::Look(const FInputActionValue& Value)
{
	const FVector2D Location = Value.Get<FVector2D>();
	
	if (APawn* CurrentPawn = GetPawn())
	{
		CurrentPawn->AddControllerYawInput(Location.X);
		CurrentPawn->AddControllerPitchInput(Location.Y);
	}
}

void AFPSCombatPlayerController::Jump()
{
	if (ACharacter* PlayerCharacter = GetCharacter())
	{
		PlayerCharacter->Jump();
	}
}

void AFPSCombatPlayerController::StopJump()
{
	if (ACharacter* PlayerCharacter = GetCharacter())
	{
		PlayerCharacter->StopJumping();
	}
}
