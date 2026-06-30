// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSCombatSystem/Public/Characters/FPSCombatCharacter.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameModes/FPSCombatPlayerState.h"


// Sets default values
AFPSCombatCharacter::AFPSCombatCharacter()
{
	GetCapsuleComponent()->SetCapsuleSize(42.0f, 96.0f);

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);


	// Don`t rotate with controller
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	FPSFollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FPSFollowCamera->SetupAttachment(GetMesh(), "Head");
	FPSFollowCamera->SetRelativeLocation(FVector(0.0f, 10.0f, 0.0f));
	FPSFollowCamera->bUsePawnControlRotation = true;

	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	HealthComponent = CreateDefaultSubobject<UFPSCombatHealthComponent>(TEXT("HealthComp"));
	
	
}



void AFPSCombatCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	SetOwner(NewController);

	InitializeAbilitySystem();
	
}


void AFPSCombatCharacter::InitializeAbilitySystem()
{
	AFPSCombatPlayerState* APlayerState = GetPlayerState<AFPSCombatPlayerState>();
	if (APlayerState)
	{
		UFPSCombatAbilitySystemComponent* AbilitySystem = Cast<UFPSCombatAbilitySystemComponent>(APlayerState->GetAbilitySystemComponent());
		
		AbilitySystem->InitAbilityActorInfo(APlayerState, this);
		
		HealthComponent->InitializeWithAbilitySystem(AbilitySystem);

		AbilitySystem->InitializeDefaultAttributes();
	}
}


