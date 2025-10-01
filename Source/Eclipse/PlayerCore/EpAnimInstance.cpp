// Fill out your copyright notice in the Description page of Project Settings.


#include "EpAnimInstance.h"
#include "EpCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void FEpDefaultAnimInstProxy::Initialize(UAnimInstance* InAnimInstance)
{
	Super::Initialize(InAnimInstance);

	if (!ensure(InAnimInstance)) return;

	AnimInstance = Cast<UEpAnimInstance>(InAnimInstance);
	OwningCharacter = Cast<AEpCharacter>(InAnimInstance->GetOwningActor());

	if (OwningCharacter)
	{
		MovementComp = OwningCharacter->GetCharacterMovement();
	}

}

void FEpDefaultAnimInstProxy::PreUpdate(UAnimInstance* InAnimInstance, float DeltaSeconds)
{
	Super::PreUpdate(InAnimInstance, DeltaSeconds);

	//SCOPE_CYCLE_COUNTER();

	if (OwningCharacter)
	{
		Acceleration = MovementComp->GetCurrentAcceleration();
		Velocity = MovementComp->GetLastUpdateVelocity();
		bIsFalling = MovementComp->IsFalling();
		bCanStrafe = !MovementComp->bOrientRotationToMovement;
	}
}

void FEpDefaultAnimInstProxy::Update(float DeltaSeconds)
{
	Super::Update(DeltaSeconds);

	if (OwningCharacter)
	{
		GroundSpeed = UKismetMathLibrary::VSizeXY(Velocity);
		bShouldMove = GroundSpeed > 3.f && false == Acceleration.Equals(FVector::ZeroVector);

		if (bCanStrafe)
		{
			Strafe = UKismetMathLibrary::Dot_VectorVector(OwningCharacter->GetActorRightVector(), UKismetMathLibrary::Normal(Velocity));
		}
	}
}

void FEpDefaultAnimInstProxy::PostUpdate(UAnimInstance* InAnimInstance) const
{
	Super::PostUpdate(InAnimInstance);


}

void UEpAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

}

void UEpAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();


}

void UEpAnimInstance::NativeUpdateAnimation(float InDeltaSeconds)
{
	Super::NativeUpdateAnimation(InDeltaSeconds);

	/*if (CachedOwnerInterface)
	{
		CachedOwnerInterface->GetCurrentAcceleration_Implementation(OUT CurrentAccelerate);
		bHasAccelation = false == CurrentAccelerate.IsZero();

		CachedOwnerInterface->GetLastUpdateVelocty_Implementation(OUT LastVelocity);
		bHasVelocty = false == LastVelocity.IsZero();
		GroundSpeed = LastVelocity.Size();

		CachedOwnerInterface->IsFalling_Implementation(OUT bIsFalling);

		FVector2D CurrentActorLocation;
		CachedOwnerInterface->GetDisplacementSinceLastUpdate_Implementation(CachedActorLocation, OUT DisplacementSinceLastUpdate, OUT CurrentActorLocation);
		CachedActorLocation = CurrentActorLocation;

		DisplacementSpeed = UKismetMathLibrary::SafeDivide(DisplacementSinceLastUpdate, GetDeltaSeconds());

		bUseBrakingFriction = CachedOwnerInterface->GetIsBrakingFriction_Implementation();
		CachedOwnerInterface->GetBrakingFriction_Implementation(OUT BrakingFriction);
		CachedOwnerInterface->GetGroundFriction_Implementation(OUT GroundFriction);
		CachedOwnerInterface->GetBrakingFrictionFactor_Implementation(OUT BrakingFrictionFactor);
		CachedOwnerInterface->GetBrakingDecelWalk_Implementation(OUT BrakingDecelWalk);
	}

	if (bStopWindow && FPlatformTime::Seconds() > StopWindowExpire)
	{
		bStopWindow = false;
	}*/

}

void UEpAnimInstance::NativeThreadSafeUpdateAnimation(float InDeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(InDeltaSeconds);

	FEpDefaultAnimInstProxy& Proxy = GetProxyOnAnyThread<FEpDefaultAnimInstProxy>();

	bIsFalling = Proxy.bIsFalling;

	Velocity = Proxy.Velocity;
	GroundSpeed = Proxy.GroundSpeed;
	bShouldMove = Proxy.bShouldMove;
	
	bCanStrafe = Proxy.bCanStrafe;
	Strafe = Proxy.Strafe;

}
