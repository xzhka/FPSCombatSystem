// FPS Combat project


#include "Weapons/Projectile/FPSCombatProjectileBase.h"

#include "GameplayCueFunctionLibrary.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

AFPSCombatProjectileBase::AFPSCombatProjectileBase()
{
	PrimaryActorTick.bCanEverTick = true;
	
	bReplicates = true;
	SetReplicateMovement(true);
	
	if (!RootComponent)
	{
		RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	}
	
	if (!CollisionComp)
	{
		CollisionComp = CreateDefaultSubobject<USphereComponent>(FName("CollisionComponent"));
		CollisionComp->SetCollisionProfileName(TEXT("Projectile"));
		CollisionComp->SetGenerateOverlapEvents(true);
		CollisionComp->InitSphereRadius(16.f);
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

		MovementComp->OnProjectileBounce.AddDynamic(this, &AFPSCombatProjectileBase::OnProjectileBounce);
	}

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComp->SetupAttachment(RootComponent);
	StaticMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	StaticMeshComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshComp(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	if (MeshComp.Succeeded())
	{
		StaticMeshComp->SetStaticMesh(MeshComp.Object);
		StaticMeshComp->SetWorldScale3D(FVector(0.3f));
	}
}

void AFPSCombatProjectileBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (HasAuthority() && StaticMeshComp && SpinRateDegPerSec > 0.f)
	{
		const FQuat DeltaSpin(SpinAxis, FMath::DegreesToRadians(SpinRateDegPerSec*DeltaSeconds));
		StaticMeshComp->AddLocalRotation(DeltaSpin);
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

	SpinAxis = FMath::VRand();
	SpinRateDegPerSec = FMath::FRandRange(180.f, 480.f);
}

UAbilitySystemComponent* AFPSCombatProjectileBase::GetSourceASC() const
{
	if (SourceASC.IsValid())
	{
		return SourceASC.Get();
	}
	return nullptr;
}

void AFPSCombatProjectileBase::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                     FVector NormalImpulse, const FHitResult& Hit)
{
	if (!OtherActor || OtherActor == this || OtherActor == GetInstigator()) return;
	
	OnProjectileImpact(OtherActor, Hit);
}

void AFPSCombatProjectileBase::OnOverlap(UPrimitiveComponent* OnComponentBeginOverlap, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == this || OtherActor == GetInstigator()) return;

	OnProjectileImpact(OtherActor, SweepResult);
}

void AFPSCombatProjectileBase::OnProjectileBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity)
{
	SpinRateDegPerSec*=0.5f;
	if (SpinRateDegPerSec < 10)
	{
		SpinRateDegPerSec = 0.f;
	}
}

void AFPSCombatProjectileBase::OnProjectileImpact(AActor* ImpactActor, const FHitResult& ImpactResult)
{
	if (bHasImpacted) return;
	bHasImpacted = true;
	
	if (bApplyDirectDamageOnImpact)
	{
		ApplyDamageToTarget(ImpactActor);
	}

	OnImpact.Broadcast(ImpactActor, ImpactResult);
	
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
	MovementComp->Friction = ProjectileDefinition->ProjectileFriction;
	MovementComp->bBounceAngleAffectsFriction = ProjectileDefinition->bProjectileBounceAffectFriction;
	MovementComp->MinFrictionFraction = ProjectileDefinition->ProjectileMinFrictionFraction;
	
	SetLifeSpan(ProjectileDefinition->ProjectileLifeSpan);
}

void AFPSCombatProjectileBase::ApplyDamageToTarget(AActor* OtherActor) const
{
	if (OtherActor && DamageSpecHandle.IsValid())
	{
		if (UAbilitySystemComponent* SourceAbilitySystem = GetSourceASC())
		{
			SourceAbilitySystem->ApplyGameplayEffectSpecToTarget(*DamageSpecHandle.Data.Get(), UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor));
		}
	}
}

void AFPSCombatProjectileBase::ApplyImpactCue(const FHitResult& ImpactResult)
{
	if (!GameplayCueExplosionTag.IsValid()) return;

	FGameplayCueParameters CueParams;
	CueParams.Location = ImpactResult.ImpactPoint;
	CueParams.Normal = ImpactResult.ImpactNormal;
	CueParams.SourceObject = this;
	CueParams.Instigator = GetInstigator();
	
	if (UAbilitySystemComponent* ASC = GetSourceASC())
	{
		ASC->ExecuteGameplayCue(GameplayCueExplosionTag, CueParams);
	}
	else
	{
		UGameplayCueFunctionLibrary::ExecuteGameplayCueOnActor(this, GameplayCueExplosionTag, CueParams);
	}
}

void AFPSCombatProjectileBase::BeginPlay()
{
	Super::BeginPlay();

	ApplyProjectileDefinition();
}