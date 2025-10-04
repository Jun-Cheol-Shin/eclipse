// Fill out your copyright notice in the Description page of Project Settings.


#include "EpCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "../Subsystems/EpInputManagerSubSystem.h"
#include "InputActionValue.h"

#include "EpPlayerController.h"
#include "../Option/EpInputConfig.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values
AEpCharacter::AEpCharacter(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;


	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 250.f;
	CameraBoom->bUsePawnControlRotation = true;


	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

}

void AEpCharacter::DoMove(float Right, float Forward)
{
	if (GetController() != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, Forward);
		AddMovementInput(RightDirection, Right);
	}
}

void AEpCharacter::DoLook(float Yaw, float Pitch)
{
	if (GetController() != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

// Called when the game starts or when spawned
void AEpCharacter::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AEpCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEpCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (AEpPlayerController* EclipseController = Cast<AEpPlayerController>(NewController))
	{
		CachedPlayerController = EclipseController;
	}

	else
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid Eclipse Player Controller!!! %s"), ANSI_TO_TCHAR(__FUNCTION__));
	}
}

void AEpCharacter::UnPossessed()
{
	CachedPlayerController.Reset();

	Super::UnPossessed();
}

// Called to bind functionality to input
void AEpCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!ensure(EnhancedInputComponent))
	{
		return;
	}

	if (!ensure(CachedPlayerController.IsValid()))
	{
		return;
	}

	UEpInputConfig* Config = CachedPlayerController->GetInputConfig();
	check(Config);

	const ULocalPlayer* LocalPlayer = CachedPlayerController->GetLocalPlayer();
	check(LocalPlayer);

	// Bind Action..
	if (const UInputAction* MoveAction = Config->FindNativeInputActionForTag(
		FGameplayTag::RequestGameplayTag(FName("InputTag.Move"), true)))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AEpCharacter::Move);
	}

	if (const UInputAction* MouseLook = Config->FindNativeInputActionForTag(
		FGameplayTag::RequestGameplayTag(FName("InputTag.MouseLook"), true)))
	{
		EnhancedInputComponent->BindAction(MouseLook, ETriggerEvent::Triggered, this, &AEpCharacter::Look);
	}




}

void AEpCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	// route the input
	DoMove(MovementVector.X, MovementVector.Y);
}

void AEpCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// route the input
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

