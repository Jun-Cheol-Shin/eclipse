// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "CommonInputTypeEnum.h"
#include "EcpInputManagerSubSystem.generated.h"

/**
 * 
 */
class FEcpInputProcessor;

DECLARE_EVENT_OneParam(FEcpInputProcessor, FChangedGamepadDetectedEvent, FName);

UCLASS()
class ECLIPSE_API UEcpInputManagerSubSystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()
	
	friend class FEcpInputProcessor;

public:
	static bool IsMobileGamepadKey(const FKey& InKey);

public:
	DECLARE_EVENT_OneParam(UEcpInputManagerSubSystem, FInputMethodChangedEvent, ECommonInputType);
	FInputMethodChangedEvent OnInputMethodChangedNative;

	FChangedGamepadDetectedEvent& GetOnGamepadChangeDetected();
	
protected:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override { return true; }
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// Gamepad function
private:
	void SetGamepadInputType(const FName& InGamepadType);
	const FName GetCurrentGamepadName() const;

private:
	void SetCurrentInputType(ECommonInputType InputType);

private:
	TSharedPtr<FEcpInputProcessor> InputProcessor = nullptr;

	UPROPERTY(Transient)
	ECommonInputType CurrentInputType;

	/**  */
	UPROPERTY(Transient)
	FName GamepadInputType;

	/**  */
	UPROPERTY(Transient)
	TMap<FName, ECommonInputType> CurrentInputLocks;

};
