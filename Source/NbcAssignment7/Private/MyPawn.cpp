#include "MyPawn.h"

#include "AirCraft.h"
#include "EnhancedInputComponent.h"
#include "MyPlayerController.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"

AMyPawn::AMyPawn()
{
	PrimaryActorTick.bCanEverTick = false;

	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComp"));
	RootComponent = CapsuleComp;

	SkeletalMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComp"));
	SkeletalMeshComp->SetupAttachment(RootComponent);

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->TargetArmLength = 200.0f;
	SpringArmComp->bUsePawnControlRotation = false;
	SpringArmComp->bInheritYaw = true;
	SpringArmComp->bInheritPitch = false;
	SpringArmComp->bInheritRoll = false;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	CameraComp->bUsePawnControlRotation = false;
}

void AMyPawn::Interact()
{
	FHitResult Hit;
	FVector Start = GetActorLocation();
	FVector End = Start + (GetActorForwardVector() * 300.0f);

	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility))
	{
		if (AAirCraft* Craft = Cast<AAirCraft>(Hit.GetActor()))
		{
			Craft->Ride(this);
		}
	}
}

void AMyPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	AMyPlayerController* PlayerController = Cast<AMyPlayerController>(GetController());

	if (!EnhancedInput || !PlayerController) return;

	if (PlayerController->MoveAction)
	{
		EnhancedInput->BindAction(PlayerController->MoveAction, ETriggerEvent::Triggered, this, &AMyPawn::Move);
	}

	if (PlayerController->LookAction)
	{
		EnhancedInput->BindAction(PlayerController->LookAction, ETriggerEvent::Triggered, this, &AMyPawn::Look);
	}
	
	if (PlayerController->InteractAction)
	{
		EnhancedInput->BindAction(PlayerController->InteractAction, ETriggerEvent::Triggered, this, &AMyPawn::Interact);
	}
}

void AMyPawn::Move(const FInputActionValue& Value)
{
	if (!Controller) return;

	const FVector MoveInput = Value.Get<FVector>();
	const float DeltaTime = GetWorld()->GetDeltaSeconds();

	const FVector LocalMoveVector = FVector(MoveInput.X, MoveInput.Y, 0.f);
	const FVector NewLocation = LocalMoveVector * MoveSpeed * DeltaTime;

	AddActorLocalOffset(NewLocation, true);
}

void AMyPawn::Look(const FInputActionValue& Value)
{
	if (!Controller) return;

	const FVector2D LookInput = Value.Get<FVector2D>();
	const float DeltaTime = GetWorld()->GetDeltaSeconds();

	const float YawChange = LookInput.X * LookSensitivity * DeltaTime;
	const float PitchChange = LookInput.Y * LookSensitivity * DeltaTime * -1.0f;

	AddActorLocalRotation(FRotator(0.f, YawChange, 0.f));

	if (SpringArmComp)
	{
		const FRotator CurrentSpringRot = SpringArmComp->GetRelativeRotation();
		const float NewPitch = FMath::Clamp(CurrentSpringRot.Pitch + PitchChange, -60.f, 60.f);
        
		SpringArmComp->SetRelativeRotation(FRotator(NewPitch, 0.f, 0.f));
	}
}