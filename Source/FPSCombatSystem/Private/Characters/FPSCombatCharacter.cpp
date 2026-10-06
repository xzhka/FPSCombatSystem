// Fill out your copyright notice in the Description page of Project Settings.
#include "FPSCombatSystem/Public/Characters/FPSCombatCharacter.h"
#include "Characters/FPSCombatMovementComp.h"
#include "Components/CapsuleComponent.h"
#include "FPSCombatSystem/FPSCombatGameplayTags.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameModes/FPSCombatGameMode.h"
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

	HealthComponent->OnDeathStarted.AddDynamic(this, &AFPSCombatCharacter::OnDeathStarted);
	HealthComponent->OnDeathEnded.AddDynamic(this, &AFPSCombatCharacter::OnDeathEnded);
	
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
	NewController->ResetIgnoreInputFlags();
	
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
			CachedASC = AbilitySystem;
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
	UFPSCombatAbilitySystemComponent* ASC = CachedASC.Get();
	CachedASC = nullptr;
	if (!ASC) return;
	
	if (ASC->GetAvatarActor() == this)
	{
		if (HasAuthority()) EquipmentComponent->UnequipAll();
		ASC->ClearAbilityInput();
		ASC->RemoveAllGameplayCues();
		MovementComponent->UninitializeFromAbilitySystem();
		HealthComponent->UninitializeFromAbilitySystem();
		StaminaComponent->UninitializeFromAbilitySystem();
		if (ASC->GetOwnerActor() != nullptr)
		{
			ASC->SetAvatarActor(nullptr);
		}
		else
		{
			ASC->ClearActorInfo();
		}
	}
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

void AFPSCombatCharacter::OnDeathStarted(AActor* OwningActor)
{
	if (HasAuthority())
	{
		FFPSCombatDeathInfo Info;
		Info.MontageIndex = FMath::RandRange(0, MAX_int32-1);
		Info.Alpha = FMath::FRand();
		Info.Velocity = GetCharacterMovement()->GetLastUpdateVelocity();
		HealthComponent->SetDeathInfo(Info);
	}
	
	if (GetController())
	{
		GetController()->SetIgnoreMoveInput(true);
		GetController()->SetIgnoreLookInput(true);
	}

	bUseControllerRotationYaw = false;

	if (UCapsuleComponent* CapsuleComp = GetCapsuleComponent())
	{
		CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		CapsuleComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	}

	if (GetCharacterMovement())
	{
		GetCharacterMovement()->StopMovementImmediately();
		GetCharacterMovement()->DisableMovement();
		GetCharacterMovement()->bOrientRotationToMovement = false;
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
	}

	K2_OnDeathStarts(HealthComponent->GetDeathInfo());
}

void AFPSCombatCharacter::OnDeathEnded(AActor* OwningActor)
{
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &AFPSCombatCharacter::ClearActorDueDeath);
}

void AFPSCombatCharacter::ClearActorDueDeath()
{
	K2_OnDeathEnds();
	
	
	if (GetLocalRole() == ROLE_Authority)
	{
		AController* DeadController = GetController();
		DetachFromControllerPendingDestroy();
		SetLifeSpan(0.1f);

		if (AFPSCombatGameMode* GM = GetWorld()->GetAuthGameMode<AFPSCombatGameMode>())
		{
			GM->HandlePawnDeath(DeadController);
		}
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