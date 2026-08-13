// Fill out your copyright notice in the Description page of Project Settings.
#include "FPSCombatSystem/Public/Characters/FPSCombatCharacter.h"
#include "Characters/FPSCombatMovementComp.h"
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

	/* Default components initialize*/
	PawnComponent = CreateDefaultSubobject<UFPSCombatCharacterPawnComp>(TEXT("PawnComponent"));
	HealthComponent = CreateDefaultSubobject<UFPSCombatHealthComponent>(TEXT("HealthComponent"));
	StaminaComponent = CreateDefaultSubobject<UFPSCombatStaminaComponent>(TEXT("StaminaComponent"));
	MovementComponent = CreateDefaultSubobject<UFPSCombatMovementComp>(TEXT("MovementComponent"));
	EquipmentComponent = CreateDefaultSubobject<UFPSCombatEquipmentManager>(TEXT("EquipmentManager"));
}

void AFPSCombatCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PawnComponent->InitializeInputComponents(PlayerInputComponent);

}

void AFPSCombatCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	SetOwner(NewController);
	
	InitializeAbilitySystem();
	EquipmentComponent->OnEquipItem(WeaponDefinition);
	EquipmentComponent->OnEquipItem(ThrowableDefinition);
}

void AFPSCombatCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	InitializeAbilitySystem();
}

void AFPSCombatCharacter::UnPossessed()
{
	Super::UnPossessed();
}

void AFPSCombatCharacter::InitializeAbilitySystem()
{
	if (AFPSCombatPlayerState* APlayerState = GetPlayerState<AFPSCombatPlayerState>())
	{
		if (UFPSCombatAbilitySystemComponent* AbilitySystem = Cast<UFPSCombatAbilitySystemComponent>(APlayerState->GetAbilitySystemComponent()))
		{
			AbilitySystem->InitAbilityActorInfo(APlayerState, this);
		
			HealthComponent->InitializeWithAbilitySystem(AbilitySystem);
			StaminaComponent->InitializeWithAbilitySystem(AbilitySystem);
			MovementComponent->InitializeWithAbilitySystem(AbilitySystem);

			if (HasAuthority())
			{
				AbilitySystem->InitializeDefaultAttributes();

				if (AbilitySet)
				{
					AbilitySet->GiveAbility(AbilitySystem, &GrantedHandles);
				}
			}
		}
	}
}

UAbilitySystemComponent* AFPSCombatCharacter::GetAbilitySystemComponent() const
{
	return GetPlayerState<AFPSCombatPlayerState>() ? GetPlayerState<AFPSCombatPlayerState>()->GetAbilitySystemComponent() : nullptr;
}

void AFPSCombatCharacter::BeginPlay()
{
	Super::BeginPlay();
	check(GetMesh());
	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		AnimInstance->LinkAnimClassLayers(DefaultAnimLayerClass);
	}
}
