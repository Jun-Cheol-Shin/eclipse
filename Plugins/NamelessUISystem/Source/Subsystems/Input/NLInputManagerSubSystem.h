// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonInputTypeEnum.h"
#include "EnhancedInputSubsystems.h"
#include "NLInputManagerSubSystem.generated.h"

/**
 * 
 */
class FNLInputProcessor;

DECLARE_EVENT_OneParam(FNLInputProcessor, FChangedGamepadDetectedEvent, FName);
DECLARE_MULTICAST_DELEGATE(FChangedInputTypeDetectedEvent)

UCLASS(MinimalAPI, Abstract)
class UNLInputManagerSubSystem : public UEnhancedInputLocalPlayerSubsystem
{
	GENERATED_BODY()
	
	friend class FNLInputProcessor;

public:
	static bool IsMobileGamepadKey(const FKey& InKey);
	void SetInputFilter(ECommonInputType InputType, bool bIsLock);

public:
	DECLARE_EVENT_OneParam(UNLInputManagerSubSystem, FInputMethodChangedEvent, ECommonInputType);
	FInputMethodChangedEvent OnInputMethodChangedNative;

	FChangedGamepadDetectedEvent& GetOnGamepadChangeDetected();

	FChangedInputTypeDetectedEvent OnChangedDetectMouseAndKeyboard;
	FChangedInputTypeDetectedEvent OnChangedDetectTouch;
	FChangedInputTypeDetectedEvent OnChangedDetectGamePad;


	
protected:
	NAMELESSUISYSTEM_API virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	NAMELESSUISYSTEM_API virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	NAMELESSUISYSTEM_API virtual void Deinitialize() override;

	// Gamepad function
private:
	void SetGamepadInputType(const FName& InGamepadType);
	const FName GetCurrentGamepadName() const;

private:
	void SetCurrentInputType(ECommonInputType InputType);

protected:
	UPROPERTY(Transient)
	ECommonInputType CurrentInputType = ECommonInputType::Count;

	UPROPERTY(Transient)
	FName GamepadInputType;

private:
	TSharedPtr<FNLInputProcessor> InputProcessor = nullptr;
};
