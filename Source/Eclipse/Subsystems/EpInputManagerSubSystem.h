// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/Input/NLInputManagerSubSystem.h"
#include "EnhancedInputSubsystemInterface.h"
#include "EpInputManagerSubSystem.generated.h"

class UInputMappingContext;
class UEnhancedPlayerInput;
class UEnhancedInputUserSettings;
class FSubsystemCollectionBase;
/**
 * 
 */
UCLASS()
class ECLIPSE_API UEpInputManagerSubSystem : public UNLInputManagerSubSystem
{
	GENERATED_BODY()

public:
	ECommonInputType GetInputType() const { return CurrentInputType; }
	const FName& GetGamepadName() const { return GamepadInputType;  }

	// Start NLInputSystem 
	// 
	//DECLARE_EVENT_OneParam(UNLInputManagerSubSystem, FInputMethodChangedEvent, ECommonInputType);
	//FInputMethodChangedEvent OnInputMethodChangedNative;

	//FChangedGamepadDetectedEvent& GetOnGamepadChangeDetected();
	//FChangedInputTypeDetectedEvent OnChangedDetectMouseAndKeyboard;
	//FChangedInputTypeDetectedEvent OnChangedDetectTouch;
	//FChangedInputTypeDetectedEvent OnChangedDetectGamePad;
	// 
	// End NLInputSystem

protected:
	// Begin ULocalPlayerSubsystem
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual void PlayerControllerChanged(APlayerController* NewPlayerController) override;
	//virtual bool ShouldCreateSubsystem(UObject* Outer) const override { return true; }
	
};
