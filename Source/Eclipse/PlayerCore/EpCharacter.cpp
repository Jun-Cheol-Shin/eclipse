// Fill out your copyright notice in the Description page of Project Settings.


#include "EpCharacter.h"
#include "EpPlayerState.h"
#include "Component/Inventory/EpInventoryComponent.h"
#include "AbilitySystemComponent.h"
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

	// AI 캐릭터는 여기서 AbilitySystemComp를 가져온다.
	// AbilitySystemComponent를 Actor가 가지고 있고 Controller가 Pawn에 Set되어야 한다는 제약도 없습니다.

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

	// 서버에서 ASC를 설정합니다. 클라이언트는 OnRep_PlayerState()에서 이 작업을 수행합니다.
	if (AEpPlayerState* EpPlayerState = GetPlayerState<AEpPlayerState>())
	{
		CachedAbilitySystemComp = EpPlayerState->GetAbilitySystemComponent();

		// AI에는 PlayerController가 없으므로 여기서 다시 초기화할 수 있습니다. 
		// PlayerController가 있는 영웅일 경우 두 번 초기화하는 것도 나쁘지 않습니다.

		// Owner Actor는 해당 Component를 가지고 있는 Actor 입니다.
		// Avatar Actor는 해당 Component를 적용받고 화면에 보여질 Actor 입니다.
		// AI (적군)은 Owner Actor와 Avatar Actor가 해당 Actor가 될 것입니다.

		if (ensure(CachedAbilitySystemComp.IsValid()))
		{
			CachedAbilitySystemComp->InitAbilityActorInfo(EpPlayerState, this);
			UE_LOG(LogTemp, Warning, TEXT("Init Ability System Component!"));
		}


		CachedInventoryComp = EpPlayerState->GetInventoryComponent();
	}
}

void AEpCharacter::OnRep_Controller()
{
	if (false == CachedPlayerController.IsValid())
	{
		CachedPlayerController = Cast<AEpPlayerController>(GetController());
	}
}

// 클라이언트만
void AEpCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if (AEpPlayerState* EpPlayerState = GetPlayerState<AEpPlayerState>())
	{
		// 클라이언트에 대한 ASC를 설정합니다. 서버는 PossessedBy에서 이 작업을 수행합니다.

		CachedAbilitySystemComp = EpPlayerState->GetAbilitySystemComponent();
		if (ensure(CachedAbilitySystemComp.IsValid()))
		{
			// 클라이언트에 대한 ASC 액터 정보를 초기화합니다. 서버가 새 액터를 보유하면 ASC를 초기화합니다.
			CachedAbilitySystemComp->InitAbilityActorInfo(EpPlayerState, this);
		}
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

