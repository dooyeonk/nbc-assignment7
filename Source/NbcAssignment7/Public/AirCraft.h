#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "AirCraft.generated.h"

struct FInputActionValue;

class UCapsuleComponent;
class UStaticMeshComponent;
class USpringArmComponent;
class UCameraComponent;
class APawn;

UCLASS()
class NBCASSIGNMENT7_API AAirCraft : public APawn
{
	GENERATED_BODY()

public:
	AAirCraft();
	
	void Ride(APawn* PlayerPawn);
	void Exit();

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	UFUNCTION()
	void Move(const FInputActionValue& Value);
	UFUNCTION()
	void Look(const FInputActionValue& Value);
	UFUNCTION()
	void Roll(const FInputActionValue& Value);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCapsuleComponent* CapsuleComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USpringArmComponent* SpringArmComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* CameraComp;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float MoveSpeed = 1500.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float RotationSpeed = 100.f;
	
	UPROPERTY()
	APawn* SavedPlayerPawn;
};
