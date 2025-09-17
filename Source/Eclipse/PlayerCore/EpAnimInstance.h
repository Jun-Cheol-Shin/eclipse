// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "EpAnimInstance.generated.h"

UENUM(BlueprintType)
enum class EMovementDirection : uint8
{
	Forward	= 0		UMETA(DisplayName = "Forward"),
	Right			UMETA(DisplayName = "Right"),
	Backward		UMETA(DisplayName = "Backward"),
	Left			UMETA(DisplayName = "Left"),

	Count,
};

/**
 * 
 */
UCLASS()
class ECLIPSE_API UEpAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
	
public:
	// 생성자
	UEpAnimInstance();

	void GetDirectionAngle();

protected:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float InDeltaSeconds) override;
	virtual void NativeThreadSafeUpdateAnimation(float InDeltaSeconds) override;
	virtual void NativePostEvaluateAnimation() override;
	virtual void NativeUninitializeAnimation() override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadwrite, Category = "Essential Movement Data", meta = (AllowPrivateAccess = "true"))
	FVector Velocity;

	UPROPERTY(VisibleAnywhere, BlueprintReadwrite, Category = "Essential Movement Data", meta = (AllowPrivateAccess = "true"))
	float GroundSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadwrite, Category = "Essential Movement Data", meta = (AllowPrivateAccess = "true"))
	bool IsFalling;

	// 캐릭터 진행 방향
	UPROPERTY(VisibleAnywhere, BlueprintReadwrite, Category = "Direction", meta = (AllowPrivateAccess = "true"))
	EMovementDirection MovementDirectionType;
};
