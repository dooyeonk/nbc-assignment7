#include "MyPlayerController.h"
#include "EnhancedInputSubsystems.h"

AMyPlayerController::AMyPlayerController()
	: InputMappingContext(nullptr), LookAction(nullptr), MoveAction(nullptr)
{
}

void AMyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (!InputMappingContext) return;

	const ULocalPlayer* LocalPlayer = GetLocalPlayer();
	if (!LocalPlayer) return;

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
	{
		Subsystem->AddMappingContext(InputMappingContext, 0);
	}
}
