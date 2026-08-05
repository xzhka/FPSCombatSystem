// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Projectile/FPSCombatProjectileBase.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

AFPSCombatProjectileBase::AFPSCombatProjectileBase()
{
	bReplicates = true;
	SetReplicateMovement(true);
	
	if (!RootComponent)
	{
		RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	}
	
	if (!CollisionComp)
	{
		CollisionComp = CreateDefaultSubobject<USphereComponent>(FName("CollisionComponent"));
		CollisionComp->BodyInstance.SetCollisionProfileName(TEXT("Projectile"));
		CollisionComp->InitSphereRadius(16.f);
		CollisionComp->SetGenerateOverlapEvents(true);
		CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AFPSCombatProjectileBase::OnOverlap);
		CollisionComp->OnComponentHit.AddDynamic(this, &AFPSCombatProjectileBase::OnHit);
		RootComponent = CollisionComp;
	}
	
	if (!MovementComp)
	{
		MovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(FName("MovementComponent"));
		MovementComp->SetUpdatedComponent(CollisionComp);
		MovementComp->bRotationFollowsVelocity = true;
		MovementComp->bShouldBounce = true;
	}

	UStaticMeshComponent* StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(RootComponent);
	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	StaticMeshComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshComp(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	if (MeshComp.Succeeded())
	{
		StaticMeshComponent->SetStaticMesh(MeshComp.Object);
		StaticMeshComponent->SetWorldScale3D(FVector(0.3f));
	}
}


void AFPSCombatProjectileBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (AActor* InstigatorActor = GetInstigator())
	{
		CollisionComp->IgnoreActorWhenMoving(InstigatorActor, true);
	}
}

void AFPSCombatProjectileBase::InitializeProjectile(const FGameplayEffectSpecHandle& SpecHandle, AActor* InstigatorActor, UAbilitySystemComponent* ASC)
{
	SourceASC = ASC;
	DamageSpecHandle = SpecHandle;

	if (InstigatorActor)
	{
		CollisionComp->IgnoreActorWhenMoving(InstigatorActor, true);
	}
}

void AFPSCombatProjectileBase::InitializeVelocity(const FVector& ProjectileVelocity)
{
	MovementComp->bInitialVelocityInLocalSpace = false;
	MovementComp->Velocity = ProjectileVelocity;
}

UAbilitySystemComponent* AFPSCombatProjectileBase::GetSourceASC() const
{
	UE_LOG(LogTemp, Warning, TEXT("GetSourceASC: (SourceASC.IsValid(): %d"), SourceASC.IsValid());
	if (SourceASC.IsValid())
	{
		return SourceASC.Get();
	}
	return nullptr;
}

void AFPSCombatProjectileBase::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                     FVector NormalImpulse, const FHitResult& Hit)
{
	OnProjectileImpact(OtherActor, Hit);
}

void AFPSCombatProjectileBase::OnOverlap(UPrimitiveComponent* OnComponentBeginOverlap, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == this || OtherActor == GetInstigator()) return;

	OnProjectileImpact(OtherActor, SweepResult);
}

void AFPSCombatProjectileBase::OnProjectileImpact(AActor* ImpactActor, const FHitResult& ImpactResult)
{
	UE_LOG(LogTemp, Warning, TEXT("OnProjectileImpact"));
	OnImpact.Broadcast(ImpactActor, ImpactResult);
	
	if (bHasImpacted) return;
	bHasImpacted = true;

	if (bApplyDirectDamageOnImpact)
	{
		ApplyDamageToTarget(ImpactActor);	
	}

	//TODO: Add function for applying VFX

	if (bDestroyOnImpact)
	{
		Destroy();	
	}
}

void AFPSCombatProjectileBase::ApplyProjectileDefinition()
{
	if (!ProjectileDefinition) return;
	
	MovementComp->InitialSpeed = ProjectileDefinition->ProjectileSpeed;
	MovementComp->MaxSpeed = ProjectileDefinition->ProjectileSpeed;
	MovementComp->Bounciness = ProjectileDefinition->ProjectileBounciness;
	MovementComp->ProjectileGravityScale = ProjectileDefinition->ProjectileGravityScale;

	SetLifeSpan(ProjectileDefinition->ProjectileLifeSpan);
}

void AFPSCombatProjectileBase::ApplyDamageToTarget(AActor* OtherActor) const
{
	if (OtherActor && DamageSpecHandle.IsValid())
	{
		if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
		{
			TargetASC->ApplyGameplayEffectSpecToTarget(*DamageSpecHandle.Data.Get(), TargetASC);
		}
	}
}

void AFPSCombatProjectileBase::BeginPlay()
{
	Super::BeginPlay();

	ApplyProjectileDefinition();
}