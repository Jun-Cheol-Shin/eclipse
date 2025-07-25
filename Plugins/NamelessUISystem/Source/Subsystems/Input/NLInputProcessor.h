// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Application/IInputProcessor.h"
#include "../Input/NLInputManagerSubSystem.h"

class FNLInputProcessor : public IInputProcessor
{

public:
	FNLInputProcessor(UNLInputManagerSubSystem& InCommonInputSubsystem)
		: InputSubsystem(InCommonInputSubsystem)
	{
		for (uint8 InputTypeIndex = 0; InputTypeIndex < (uint8)ECommonInputType::Count; InputTypeIndex++)
		{
			InputMethodPermissions[InputTypeIndex] = false;
		}
	}
	virtual ~FNLInputProcessor() {};

	void SetInputFilter(ECommonInputType InputType, bool bIsLock);


	FChangedGamepadDetectedEvent OnGamepadChangeDetected;


protected:
	virtual void Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor) override {};

	/** Key down input */
	virtual bool HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) override;

	/** Key up input */
	//virtual bool HandleKeyUpEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) override;

	/** Analog axis input */
	virtual bool HandleAnalogInputEvent(FSlateApplication& SlateApp, const FAnalogInputEvent& InAnalogInputEvent) override;

	/** Mouse movement input */
	virtual bool HandleMouseMoveEvent(FSlateApplication& SlateApp, const FPointerEvent& InMouseEvent) override;

	/** Mouse button press */
	virtual bool HandleMouseButtonDownEvent(FSlateApplication& SlateApp, const FPointerEvent& InMouseEvent) override;

	/** Mouse button double clicked. */
	virtual bool HandleMouseButtonDoubleClickEvent(FSlateApplication& SlateApp, const FPointerEvent& InMouseEvent) override;

	/** Mouse wheel input */
	virtual bool HandleMouseWheelOrGestureEvent(FSlateApplication& SlateApp, const FPointerEvent& InWheelEvent, const FPointerEvent* InGestureEvent) override;


private:
	bool IsRelevantInput(FSlateApplication& SlateApp, const FInputEvent& InputEvent, const ECommonInputType DesiredInputType);

	void RefreshCurrentInputMethod(ECommonInputType InputMethod);

	ECommonInputType GetInputType(const FKey& InKey);
	ECommonInputType GetInputType(const FPointerEvent& InPointerEvent);

	bool IsInputMethodBlocked(ECommonInputType InputType) const;


private:
	UNLInputManagerSubSystem& InputSubsystem;

	bool InputMethodPermissions[(uint8)ECommonInputType::Count];

	FName LastSeenGamepadInputDeviceName;
	FString LastSeenGamepadHardwareDeviceIdentifier;
};
