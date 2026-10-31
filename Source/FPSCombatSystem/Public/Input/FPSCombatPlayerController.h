// FPS Combat project

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Equipment/FPSCombatQuickBarComponent.h"
#include "FPSCombatPlayerController.generated.h"

struct FInputActionValue;
class UInputMappingContext;
class UInputAction;

/*	AFPSCombatPlayerController
 *	
 *	Main base project controller class
 */
UCLASS()
class FPSCOMBATSYSTEM_API AFPSCombatPlayerController : public APlayerController, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

	AFPSCombatPlayerController(const FObjectInitializer& ObjectInitializer);

public:
	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;

	virtual void SetGenericTeamId(const FGenericTeamId& TeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override;
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "QuickBar")
	TObjectPtr<UFPSCombatQuickBarComponent> QuickBarComponent;

	
	virtual void BeginPlay() override;
	
	
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> HUDWidgetClass;

	UPROPERTY()
	TObjectPtr<UUserWidget> HUDWidgetInstance;
	
};
