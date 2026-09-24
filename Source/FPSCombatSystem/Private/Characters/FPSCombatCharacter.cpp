// Fill out your copyright notice in the Description page of Project Settings.
#include "FPSCombatSystem/Public/Characters/FPSCombatCharacter.h"
#include "Characters/FPSCombatMovementComp.h"
#include "Components/CapsuleComponent.h"
#include "FPSCombatSystem/FPSCombatGameplayTags.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameModes/FPSCombatPlayerState.h"
#include "Weapons/Projectile/FPSCombatThrowableInstance.h"


// Sets default values
AFPSCombatCharacter::AFPSCombatCharacter()
{
	bReplicates = true;
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
	RigIKComponent = CreateDefaultSubobject<UFPSCombatRigIKComponent>(TEXT("RigIK"));
}

void AFPSCombatCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PawnComponent->InitializeInputComponents(PlayerInputComponent);
}

void AFPSCombatCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UninitializeAbilitySystem();
	Super::EndPlay(EndPlayReason);
}

void AFPSCombatCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	SetOwner(NewController);
	
	InitializeAbilitySystem();
	GrantDefaultEquipment();
}

void AFPSCombatCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	InitializeAbilitySystem();
}

void AFPSCombatCharacter::UnPossessed()
{
	UninitializeAbilitySystem();
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

				APlayerState->GrantDefaultAbilities();
			}
		}
	}
}

void AFPSCombatCharacter::UninitializeAbilitySystem()
{
	HealthComponent->UninitializeFromAbilitySystem();
	StaminaComponent->UninitializeFromAbilitySystem();
}

UAbilitySystemComponent* AFPSCombatCharacter::GetAbilitySystemComponent() const
{
	return GetPlayerState<AFPSCombatPlayerState>() ? GetPlayerState<AFPSCombatPlayerState>()->GetAbilitySystemComponent() : nullptr;
}

void AFPSCombatCharacter::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	check(GetMesh());

	if (DefaultAnimClass)
	{
		GetMesh()->LinkAnimClassLayers(DefaultAnimClass);
	}
}

void AFPSCombatCharacter::GrantDefaultEquipment()
{
	if (ThrowableDefinition == nullptr)
	{
		return;
	}

	AFPSCombatPlayerState* PS = GetPlayerState<AFPSCombatPlayerState>();
	if (!PS) return;
	
	UFPSCombatItemInstance* ItemInstance = PS->GetComponentByClass<UFPSCombatItemManagerComponent>()->AddStack(ThrowableItemDefinition, FPSCombatGameplayTags::Data_Projectile_Quantity, 0);
	if (UFPSCombatEquipmentInstance* ThrowableInstance = EquipmentComponent->OnEquipItem(ThrowableDefinition, ItemInstance))
	{
		ThrowableInstance->SetEquipmentActorsHidden(true);
	}
}