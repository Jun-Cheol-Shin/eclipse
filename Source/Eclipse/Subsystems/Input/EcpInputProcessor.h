// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Application/IInputProcessor.h"

/**
 * 
 */
class ECLIPSE_API FEcpInputProcessor : public IInputProcessor
{
public:
	FEcpInputProcessor() {};
	virtual ~FEcpInputProcessor() {};

protected:
	virtual void Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor) {};

	/** Key down input */
	virtual bool HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) { return false; }

	/** Key up input */
	virtual bool HandleKeyUpEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) { return false; }

	/** Analog axis input */
	virtual bool HandleAnalogInputEvent(FSlateApplication& SlateApp, const FAnalogInputEvent& InAnalogInputEvent) { return false; }

	/** Mouse movement input */
	virtual bool HandleMouseMoveEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) { return false; }

	/** Mouse button press */
	virtual bool HandleMouseButtonDownEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) { return false; }

	/** Mouse button release */
	virtual bool HandleMouseButtonUpEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) { return false; }

	/** Mouse button double clicked. */
	virtual bool HandleMouseButtonDoubleClickEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) { return false; }

	/** Mouse wheel input */
	virtual bool HandleMouseWheelOrGestureEvent(FSlateApplication& SlateApp, const FPointerEvent& InWheelEvent, const FPointerEvent* InGestureEvent) { return false; }

	/** Called when a motion-driven device has new input */
	virtual bool HandleMotionDetectedEvent(FSlateApplication& SlateApp, const FMotionEvent& MotionEvent) { return false; };

	/** Debug name for logging purposes */
	virtual const TCHAR* GetDebugName() const { return TEXT(""); }

};
