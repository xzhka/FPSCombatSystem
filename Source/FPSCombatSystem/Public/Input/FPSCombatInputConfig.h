// FPS Combat project

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "FPSCombatInputConfig.generated.h"


class UInputAction;
class UInputMappingContext;

/*	FPSInputAction
 *	
 *	Struct which connects the action and tag
 */
USTRUCT(BlueprintType)
struct FPSInputAction
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<const UInputAction> BaseInputActions = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag InputTag;
};

/*	UFPSCombatInputConfig
 *	
 *	Data asset which contains an input actions
 */
UCLASS()
class FPSCOMBATSYSTEM_API UFPSCombatInputConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TArray<FPSInputAction> InputActions;

	
};
