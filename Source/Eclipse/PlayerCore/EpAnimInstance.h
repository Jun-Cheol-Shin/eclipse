// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimInstanceProxy.h"
#include "EpAnimInstance.generated.h"


struct FAnimInstanceProxy;

class UCharacterMovementComponent;
class AEpCharacter;
class UEpAnimInstance;

USTRUCT(BlueprintType)
struct FEpDefaultAnimInstProxy : public FAnimInstanceProxy
{
	GENERATED_BODY()

	friend class UEpAnimInstance;

public:
	FEpDefaultAnimInstProxy() : FAnimInstanceProxy() {}
	FEpDefaultAnimInstProxy(UAnimInstance* InAnimInstance) : FAnimInstanceProxy(InAnimInstance) {}

protected:
	/** Called when our anim instance is being initialized */
	virtual void Initialize(UAnimInstance* InAnimInstance) override;

	/** Called before update so we can copy any data we need */
	virtual void PreUpdate(UAnimInstance* InAnimInstance, float DeltaSeconds) override;



	/** Update override point */
	virtual void Update(float DeltaSeconds) override;

	/** Called after update so we can copy any data we need */
	virtual void PostUpdate(UAnimInstance* InAnimInstance) const override;

	/** Called when our anim instance is being uninitialized */
	//virtual void Uninitialize(UAnimInstance* InAnimInstance) override;

private:
	UPROPERTY(Transient, BlueprintReadWrite, EditAnywhere, Category = "Default Anim Instance Proxy Value", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UEpAnimInstance> AnimInstance = nullptr;

	UPROPERTY(Transient, BlueprintReadWrite, EditAnywhere, Category = "Default Anim Instance Proxy Value", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<AEpCharacter> OwningCharacter = nullptr;

	UPROPERTY(Transient, BlueprintReadWrite, EditAnywhere, Category = "Default Anim Instance Proxy Value", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCharacterMovementComponent> MovementComp = nullptr;

	UPROPERTY(Transient, BlueprintReadWrite, EditAnywhere, Category = "Default Anim Instance Proxy Value", meta = (AllowPrivateAccess = "true"))
	FVector Velocity = FVector::ZeroVector;

	UPROPERTY(Transient, BlueprintReadWrite, EditAnywhere, Category = "Default Anim Instance Proxy Value", meta = (AllowPrivateAccess = "true"))
	FVector Acceleration = FVector::ZeroVector;

	UPROPERTY(Transient, BlueprintReadWrite, EditAnywhere, Category = "Default Anim Instance Proxy Value", meta = (AllowPrivateAccess = "true"))
	double GroundSpeed = 0.f;

	UPROPERTY(Transient, BlueprintReadWrite, EditAnywhere, Category = "Default Anim Instance Proxy Value", meta = (AllowPrivateAccess = "true"))
	bool bIsFalling = false;

	UPROPERTY(Transient, BlueprintReadWrite, EditAnywhere, Category = "Default Anim Instance Proxy Value", meta = (AllowPrivateAccess = "true"))
	bool bShouldMove = false;

	UPROPERTY(Transient, BlueprintReadWrite, EditAnywhere, Category = "Default Anim Instance Proxy Value", meta = (AllowPrivateAccess = "true"))
	bool bCanStrafe = false;

	UPROPERTY(Transient, BlueprintReadWrite, EditAnywhere, Category = "Default Anim Instance Proxy Value", meta = (AllowPrivateAccess = "true"))
	float Strafe = 0.f;

};


/**
 * 
 */
UCLASS()
class ECLIPSE_API UEpAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UEpAnimInstance() {}

protected:
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Anim Instance Proxy", meta = (DisplayName = "Anim Instance Proxy"))
	FEpDefaultAnimInstProxy DefaultAnimInstProxy;

	virtual FAnimInstanceProxy* CreateAnimInstanceProxy() override { return &DefaultAnimInstProxy; }
	virtual void DestroyAnimInstanceProxy(FAnimInstanceProxy* InProxy) override {}


protected:
	virtual void NativeBeginPlay() override;
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float InDeltaSeconds) override;
	virtual void NativeThreadSafeUpdateAnimation(float InDeltaSeconds) override;


private:
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Essential Movement Data", meta = (DisplayName = "Velocity", AllowPrivateAccess = "true"))
	FVector Velocity = FVector::ZeroVector;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Essential Movement Data", meta = (DisplayName = "GroundSpeed", AllowPrivateAccess = "true"))
	double GroundSpeed = 0.f;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Essential Movement Data", meta = (DisplayName = "ShouldMove", AllowPrivateAccess = "true"))
	bool bShouldMove = false;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Essential Movement Data", meta = (DisplayName = "IsFalling", AllowPrivateAccess = "true"))
	bool bIsFalling = false;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Essential Movement Data", meta = (DisplayName = "Strafe", AllowPrivateAccess = "true"))
	float Strafe = 0.f;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Essential Movement Data", meta = (DisplayName = "CanStrafe", AllowPrivateAccess = "true"))
	bool bCanStrafe = false;
};
