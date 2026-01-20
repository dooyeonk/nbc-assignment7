#include "AirCraft.h"

#include "EnhancedInputComponent.h"
#include "MyPlayerController.h"
#include "SceneRenderTargetParameters.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/SpringArmComponent.h"

AAirCraft::AAirCraft()
{
	PrimaryActorTick.bCanEverTick = true;
	
	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	RootComponent = BoxComp;
	BoxComp->SetCollisionProfileName(TEXT("Pawn"));
	BoxComp->SetUseCCD(true);
	
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComponent);
	
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->SocketOffset = FVector(0.0f, 0.0f, 100.0f);
	SpringArmComp->TargetArmLength = 500.0f;
	SpringArmComp->bUsePawnControlRotation = false;
	SpringArmComp->bInheritPitch = true;
	SpringArmComp->bInheritYaw = true;
	SpringArmComp->bInheritRoll = true;
	
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	CameraComp->bUsePawnControlRotation = false;

}

void AAirCraft::Ride(APawn* PlayerPawn)
{
	SavedPlayerPawn = PlayerPawn;
	if (AMyPlayerController* PlayerController = Cast<AMyPlayerController>(PlayerPawn->GetController()))
	{
		Velocity = FVector::ZeroVector;
		SavedPlayerPawn->SetActorHiddenInGame(true);
		SavedPlayerPawn->SetActorEnableCollision(false);
		
		PlayerController->Possess(this);
		PlayerController->SwitchMappingContext(true);
	}
}

void AAirCraft::Exit()
{
	if (AMyPlayerController* PlayerController = Cast<AMyPlayerController>(GetController()))
	{
		const FVector ExitLocation = GetActorLocation() + (GetActorRightVector() * 200.0f);
		SavedPlayerPawn->SetActorLocation(ExitLocation);
		SavedPlayerPawn->SetActorHiddenInGame(false);
		SavedPlayerPawn->SetActorEnableCollision(true);
		
		PlayerController->Possess(SavedPlayerPawn);
		PlayerController->SwitchMappingContext(false);
	}
}

void AAirCraft::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (AMyPlayerController* PlayerController = Cast<AMyPlayerController>(GetController()))
		{
			if (PlayerController->LookAction)
			{
				EnhancedInput->BindAction(PlayerController->LookAction, ETriggerEvent::Triggered, this, &AAirCraft::Look);
			}
			
			if (PlayerController->MoveAction)
			{
				EnhancedInput->BindAction(PlayerController->MoveAction, ETriggerEvent::Triggered, this, &AAirCraft::Move);
			}
			
			if (PlayerController->RollAction)
			{
				EnhancedInput->BindAction(PlayerController->RollAction, ETriggerEvent::Triggered, this, &AAirCraft::Roll);
			}
			
			if (PlayerController->InteractAction)
			{
				EnhancedInput->BindAction(PlayerController->InteractAction, ETriggerEvent::Started, this, &AAirCraft::Exit);
			}
		}
	}
}

void AAirCraft::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	CheckGround();
	
	if (GetController())
	{
		Velocity.Z += 0.f;
	}
	else
	{
		Velocity.Z += GravityConstant * DeltaTime;
	}

	if (!Velocity.IsNearlyZero())
	{
		AddActorWorldOffset(Velocity * DeltaTime, true);
	}
}

void AAirCraft::Move(const FInputActionValue& Value)
{
	if (!Controller) return;
    
	const FVector MoveInput = Value.Get<FVector>();
	const float DeltaTime = GetWorld()->GetDeltaSeconds();

	const float CurrentSpeed = bIsFlightMode ? (MoveSpeed * AirControlMultiplier) : MoveSpeed;
	
	FVector NewLocation = MoveInput * CurrentSpeed * DeltaTime;
	AddActorLocalOffset(NewLocation, true);
}

void AAirCraft::Look(const FInputActionValue& Value)
{
	if (!Controller) return;
	
	// (x: yaw, y: pitch, z: roll)
	const FVector LookInput = Value.Get<FVector>();
	const float DeltaTime = GetWorld()->GetDeltaSeconds();

	const float Pitch = LookInput.Y * RotationSpeed * DeltaTime * -1;
	const float Yaw = LookInput.X * RotationSpeed * DeltaTime;
	const float Roll = LookInput.Z * RotationSpeed * DeltaTime;
	
	AddActorLocalRotation(FRotator(Pitch, Yaw, Roll));
}

void AAirCraft::Roll(const FInputActionValue& Value)
{
	const float RollInput = Value.Get<float>();
	const float DeltaTime = GetWorld()->GetDeltaSeconds();
	
	if (FMath::IsNearlyZero(RollInput)) return;
	
	AddActorLocalRotation(FRotator(0.f, 0.f, RollInput * RotationSpeed * DeltaTime));
}

void AAirCraft::CheckGround()
{
	FHitResult Hit;
	FVector Start = GetActorLocation();
	float BoxHalfHeight = BoxComp->GetScaledBoxExtent().Z;
	float TraceDistance = BoxHalfHeight + 20.0f;
	FVector End = Start + (FVector::DownVector * TraceDistance);
	
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
	{
		bIsFlightMode = false;
		
		FVector NewLocation = GetActorLocation();
		NewLocation.Z = Hit.Location.Z + BoxHalfHeight + 10.0f;
		SetActorLocation(NewLocation);
	}
	else
	{
		bIsFlightMode = true;
	}
}



