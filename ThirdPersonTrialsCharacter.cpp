// Copyright Epic Games, Inc. All Rights Reserved.

#include "ThirdPersonTrialsCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"
#include "Math/Color.h"
#include "ThirdPersonTrialsGameMode.h"
//////////////////////////////////////////////////////////////////////////
// AThirdPersonTrialsCharacter

AThirdPersonTrialsCharacter::AThirdPersonTrialsCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.4f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}


//////////////////////////////////////////////////////////////////////////
// Input

void AThirdPersonTrialsCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AThirdPersonTrialsCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &AThirdPersonTrialsCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AThirdPersonTrialsCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AThirdPersonTrialsCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AThirdPersonTrialsCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AThirdPersonTrialsCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AThirdPersonTrialsCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AThirdPersonTrialsCharacter::OnResetVR);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AThirdPersonTrialsCharacter::StartSprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AThirdPersonTrialsCharacter::EndSprint);
	PlayerInputComponent->BindAction("ChargeJump", IE_Pressed, this, &AThirdPersonTrialsCharacter::ChargeJump);
	PlayerInputComponent->BindAction("ResetJumpSpeed", IE_Pressed, this, &AThirdPersonTrialsCharacter::ResetJumpSpeed);
}

void AThirdPersonTrialsCharacter::Tick(float DeltaTime)
{
	GEngine->AddOnScreenDebugMessage(0, 0.1f, FColor::Blue, FString::Printf(TEXT("Stamina: %.3f out of %.3f"), CurrentStamina, TotalStamina));
	//GEngine->AddOnScreenDebugMessage(5, 0.1f, FColor::Orange, FString::Printf(TEXT("Current Speed %f"), GetCharacterMovement()->MaxWalkSpeed));

	//High Jump System
	if (bHighJumpSet)
	{
		if (!(GetCharacterMovement()->IsFalling()) && (GetCharacterMovement()->JumpZVelocity != HighJumpVelocity))
		{
			GetCharacterMovement()->JumpZVelocity = HighJumpVelocity;
		}
	}
	else
	{
		GetCharacterMovement()->JumpZVelocity = RegularJumpVelocity;
	}

	//Staimna System
	if ((CurrentStamina < TotalStamina) && !bIsSprinting)
	{
		CurrentStamina += StaminaRegenRate * DeltaTime;
	}
	else if ((CurrentStamina > 0.f) && bIsSprinting)
	{
		CurrentStamina -= StaminaConsumptionSprintRate * DeltaTime;
	}
	else if (CurrentStamina > TotalStamina)
	{
		CurrentStamina = TotalStamina;
	}
	else if(!bSpeedUpPickedUp)
	{
		EndSprint();
	}
	
}


void AThirdPersonTrialsCharacter::OnResetVR()
{
	// If ThirdPersonTrials is added to a project via 'Add Feature' in the Unreal Editor the dependency on HeadMountedDisplay in ThirdPersonTrials.Build.cs is not automatically propagated
	// and a linker error will result.
	// You will need to either:
	//		Add "HeadMountedDisplay" to [YourProject].Build.cs PublicDependencyModuleNames in order to build successfully (appropriate if supporting VR).
	// or:
	//		Comment or delete the call to ResetOrientationAndPosition below (appropriate if not supporting VR)
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AThirdPersonTrialsCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
		Jump();
}

void AThirdPersonTrialsCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
		StopJumping();
}

void AThirdPersonTrialsCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AThirdPersonTrialsCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AThirdPersonTrialsCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AThirdPersonTrialsCharacter::MoveRight(float Value)
{
	if ( (Controller != nullptr) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void AThirdPersonTrialsCharacter::StartSprint()
{
	if (CurrentStamina > StaminaConsumptionSprintRate && (!bSpeedUpPickedUp))
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
		bIsSprinting = true;
	}
}

void AThirdPersonTrialsCharacter::EndSprint()
{
	if (!bSpeedUpPickedUp)
	{
		GetCharacterMovement()->MaxWalkSpeed = 600.f;
		bIsSprinting = false;
	}
	
}

void AThirdPersonTrialsCharacter::ChargeJump()
{
	UE_LOG(LogTemp, Warning, TEXT("Jump Charged!"));
	GetCharacterMovement()->JumpZVelocity = ChargedJumpZVelocity;
}

void AThirdPersonTrialsCharacter::ResetJumpSpeed()
{
	UE_LOG(LogTemp, Warning, TEXT("Jump Reset!"));
	GetCharacterMovement()->JumpZVelocity = 600.f;
}

void AThirdPersonTrialsCharacter::SpeedUpPickupTimerFunction()
{
	UE_LOG(LogTemp, Warning, TEXT("Timer Function Called!"));
	GetWorldTimerManager().SetTimer(PickupTimerHandle, this, &AThirdPersonTrialsCharacter::RevertSpeed, 3.f, false, 5.f);
}

void AThirdPersonTrialsCharacter::SpeedUp()
{
	UE_LOG(LogTemp, Warning, TEXT("Speed Up!"));
	GetCharacterMovement()->MaxWalkSpeed = (SprintSpeed * 2);
	GEngine->AddOnScreenDebugMessage(4, 2, FColor::Purple, FString::Printf(TEXT("Speed UP! Current Speed: %f"), GetCharacterMovement()->MaxWalkSpeed));
}

void AThirdPersonTrialsCharacter::RevertSpeed()
{
	bSpeedUpPickedUp = false;
	UE_LOG(LogTemp, Warning, TEXT("Speed Reverted"))
	GetCharacterMovement()->MaxWalkSpeed = 600.f;
	GEngine->AddOnScreenDebugMessage(4, 2, FColor::Purple, FString::Printf(TEXT("Speed Reverted Current Speed: %f"), GetCharacterMovement()->MaxWalkSpeed));

}

void AThirdPersonTrialsCharacter::ShowCoins()
{
	GEngine->AddOnScreenDebugMessage(1, 3, FColor::Green, FString::Printf(TEXT("Coins: %d"),PickedUpCoins));
}

void AThirdPersonTrialsCharacter::Jump()
{
	ACharacter::Jump();
	if (bHighJumpSet)
	{
		bHighJumpSet = false;
	}
}
