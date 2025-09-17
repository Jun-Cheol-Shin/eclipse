// Fill out your copyright notice in the Description page of Project Settings.


#include "EpAnimInstance.h"

UEpAnimInstance::UEpAnimInstance()
{
	Velocity = FVector3d();
	GroundSpeed = 0;
	IsFalling = false;
	MovementDirectionType = EMovementDirection::Forward;
}

void UEpAnimInstance::GetDirectionAngle()
{
}

void UEpAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();


}

void UEpAnimInstance::NativeUpdateAnimation(float InDeltaSeconds)
{
	Super::NativeUpdateAnimation(InDeltaSeconds);


}

void UEpAnimInstance::NativeThreadSafeUpdateAnimation(float InDeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(InDeltaSeconds);


}

void UEpAnimInstance::NativePostEvaluateAnimation()
{
	Super::NativePostEvaluateAnimation();


}

void UEpAnimInstance::NativeUninitializeAnimation()
{


	Super::NativeUninitializeAnimation();
}
