// FPS Combat project

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FPSCombatWeaponActor.generated.h"

class UAnimMontage;

/*	AFPSCombatWeaponActor
 *	
 *	Actor represented weapon
 */
UCLASS()
class FPSCOMBATSYSTEM_API AFPSCombatWeaponActor : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* WeaponMeshComponent;
	
public:	
	AFPSCombatWeaponActor();

	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMeshComponent; } 
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Transform")
	FTransform GetSocketTransform(FName SocketName) const;
};
