#include "MyPlayerController.h"
#include "EnhancedInputSubsystems.h"

AMyPlayerController::AMyPlayerController()
	: InputMappingContext(nullptr), AirCraftInputMappingContext(nullptr), LookAction(nullptr), MoveAction(nullptr),
	  RollAction(nullptr), InteractAction(nullptr)
{
}

void AMyPlayerController::BeginPlay()
{
	Super::BeginPlay();
	SwitchMappingContext(false);
}

void AMyPlayerController::SwitchMappingContext(bool bIsAirCraft)
{
	const ULocalPlayer* LocalPlayer = GetLocalPlayer();
	if (!LocalPlayer) return;

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
	{
		Subsystem->ClearAllMappings();

		if (const UInputMappingContext* ContextToApply =
			bIsAirCraft ? AirCraftInputMappingContext : InputMappingContext)
		{
			Subsystem->AddMappingContext(ContextToApply, 0);
		}
	}
}
