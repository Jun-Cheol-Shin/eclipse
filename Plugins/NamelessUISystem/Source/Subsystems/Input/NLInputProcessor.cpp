// Fill out your copyright notice in the Description page of Project Settings.


#include "NLInputProcessor.h"
#include "NLInputManagerSubSystem.h"
#include "ICommonInputModule.h"

#include "Engine/Engine.h"
#include "Engine/LocalPlayer.h"
#include "Engine/GameViewportClient.h"

#include "Widgets/SViewport.h"

#include "CommonInputBaseTypes.h"

bool FNLInputProcessor::IsRelevantInput(FSlateApplication& SlateApp, const FInputEvent& InputEvent, const ECommonInputType DesiredInputType)
{

#if WITH_EDITOR
	if (GIntraFrameDebuggingGameThread)
	{
		return false;
	}
#endif

	if (SlateApp.IsActive()
		|| SlateApp.GetHandleDeviceInputWhenApplicationNotActive()
		|| (ICommonInputModule::GetSettings().GetAllowOutOfFocusDeviceInput() && DesiredInputType == ECommonInputType::Gamepad))
	{
		const ULocalPlayer& LocalPlayer = *InputSubsystem.GetLocalPlayerChecked();
		int32 ControllerId = LocalPlayer.GetControllerId();

#if WITH_EDITOR
		GEngine->RemapGamepadControllerIdForPIE(LocalPlayer.ViewportClient, ControllerId);
#endif
		return ControllerId == InputEvent.GetUserIndex();
	}

	return false;
}

void FNLInputProcessor::RefreshCurrentInputMethod(ECommonInputType InputMethod)
{
#if WITH_EDITOR
	// Lots of special-case fun for PIE - there are special scenarios wherein we want to ignore the update attempt
	const ULocalPlayer& LocalPlayer = *InputSubsystem.GetLocalPlayerChecked();
	bool bCanApplyInputMethodUpdate = false;
	if (LocalPlayer.ViewportClient)
	{
		TSharedPtr<FSlateUser> SlateUser = FSlateApplication::Get().GetUser(LocalPlayer.GetControllerId());
		if (ensure(SlateUser))
		{
			bool bRoutingGamepadToNextPlayer = false;
			if (!GetDefault<ULevelEditorPlaySettings>()->GetRouteGamepadToSecondWindow(bRoutingGamepadToNextPlayer))
			{
				// This looks strange - it's because the getter will set the output param based on the value of the setting, but return
				//	whether the setting actually matters. So we're making sure that even if the setting is true, we revert to false when it's irrelevant.
				bRoutingGamepadToNextPlayer = false;
			}

			if (SlateUser->IsWidgetInFocusPath(LocalPlayer.ViewportClient->GetGameViewportWidget()))
			{

				// Our owner's game viewport is in the focus path, which in a PIE scenario means we shouldn't
				// acknowledge gamepad input if it's being routed to another PIE client
				if (InputMethod != ECommonInputType::Gamepad || !bRoutingGamepadToNextPlayer)
				{
					bCanApplyInputMethodUpdate = true;
				}
			}
			else if (InputMethod == ECommonInputType::Gamepad)
			{
				bCanApplyInputMethodUpdate = bRoutingGamepadToNextPlayer;
			}
		}
	}
	if (!bCanApplyInputMethodUpdate)
	{
		return;
	}
#endif

	InputSubsystem.SetCurrentInputType(InputMethod);

	// Try to auto-detect the type of gamepad
	if (InputMethod == ECommonInputType::Gamepad
		&& ICommonInputModule::GetSettings().GetEnableAutomaticGamepadTypeDetection()
		&& UCommonInputPlatformSettings::Get()->CanChangeGamepadType())
	{
		if (const FInputDeviceScope* DeviceScope = FInputDeviceScope::GetCurrent())
		{
			if ((DeviceScope->InputDeviceName != LastSeenGamepadInputDeviceName) || (DeviceScope->HardwareDeviceIdentifier != LastSeenGamepadHardwareDeviceIdentifier))
			{
				LastSeenGamepadInputDeviceName = DeviceScope->InputDeviceName;
				LastSeenGamepadHardwareDeviceIdentifier = DeviceScope->HardwareDeviceIdentifier;

				const FName GamepadInputType = InputSubsystem.GetCurrentGamepadName();
				const FName BestGamepadType = UCommonInputPlatformSettings::Get()->GetBestGamepadNameForHardware(GamepadInputType, DeviceScope->InputDeviceName, DeviceScope->HardwareDeviceIdentifier);
				if (BestGamepadType != GamepadInputType)
				{
					InputSubsystem.SetGamepadInputType(BestGamepadType);
					OnGamepadChangeDetected.Broadcast(BestGamepadType);
				}
			}
		}
	}
}

ECommonInputType FNLInputProcessor::GetInputType(const FKey& InKey)
{
	// If the key is a gamepad key or if the key is a Click key (which simulates a click), we should be in Gamepad mode
	if (InKey.IsGamepadKey())
	{
		if (UNLInputManagerSubsystem::IsMobileGamepadKey(InKey))
		{
			return ECommonInputType::Touch;
		}

		return ECommonInputType::Gamepad;
	}

	return ECommonInputType::MouseAndKeyboard;
}


ECommonInputType FNLInputProcessor::GetInputType(const FPointerEvent& InPointerEvent)
{
#if !UE_BUILD_SHIPPING
	const ULocalPlayer& LocalPlayer = *InputSubsystem.GetLocalPlayerChecked();
#endif

	if (InPointerEvent.IsTouchEvent())
	{
		return ECommonInputType::Touch;
	}
	else if (ECommonInputType::Gamepad == GetInputType(InPointerEvent.GetEffectingButton()))
	{
		return ECommonInputType::Gamepad;
	}
#if !UE_BUILD_SHIPPING
	else if ((LocalPlayer.ViewportClient && LocalPlayer.ViewportClient->GetUseMouseForTouch()) || FSlateApplication::Get().IsFakingTouchEvents())
	{
		return ECommonInputType::Touch;
	}
#endif
	return ECommonInputType::MouseAndKeyboard;
}

void FNLInputProcessor::SetInputFilter(ECommonInputType InputType, bool bIsLock)
{
	InputMethodPermissions[(uint8)InputType] = bIsLock;
}

bool FNLInputProcessor::HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent)
{
	const ECommonInputType InputType = GetInputType(InKeyEvent.GetKey());
	if (IsRelevantInput(SlateApp, InKeyEvent, InputType))
	{
		if (IsInputMethodBlocked(InputType))
		{
			return true;
		}

		RefreshCurrentInputMethod(InputType);
	}
	return false;
}

bool FNLInputProcessor::HandleAnalogInputEvent(FSlateApplication& SlateApp, const FAnalogInputEvent& InAnalogInputEvent)
{
	return false;
}

bool FNLInputProcessor::HandleMouseMoveEvent(FSlateApplication& SlateApp, const FPointerEvent& InMouseEvent)
{
	const ECommonInputType InputType = GetInputType(InMouseEvent);
	if (IsRelevantInput(SlateApp, InMouseEvent, InputType))
	{
		/*if (bIgnoreNextMove)
		{
			bIgnoreNextMove = false;
		}*/

		if (!InMouseEvent.GetCursorDelta().IsNearlyZero())
		{
			if (IsInputMethodBlocked(InputType))
			{
				return true;
			}

			RefreshCurrentInputMethod(InputType);
		}
	}

	return false;
}

bool FNLInputProcessor::HandleMouseButtonDownEvent(FSlateApplication& SlateApp, const FPointerEvent& InMouseEvent)
{
	const ECommonInputType InputType = GetInputType(InMouseEvent);
	if (IsRelevantInput(SlateApp, InMouseEvent, InputType))
	{
		if (IsInputMethodBlocked(InputType))
		{
			return true;
		}

		RefreshCurrentInputMethod(InputType);
	}
	return false;
}

bool FNLInputProcessor::HandleMouseButtonDoubleClickEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent)
{
	const ECommonInputType InputType = GetInputType(MouseEvent);
	if (IsRelevantInput(SlateApp, MouseEvent, InputType))
	{
		if (IsInputMethodBlocked(InputType))
		{
			return true;
		}

		RefreshCurrentInputMethod(InputType);
	}
	return false;
}

bool FNLInputProcessor::HandleMouseWheelOrGestureEvent(FSlateApplication& SlateApp, const FPointerEvent& InWheelEvent, const FPointerEvent* InGestureEvent)
{
	/*const FPointerEvent& EventToConsider = InGestureEvent ? *InGestureEvent : InWheelEvent;
	const ECommonInputType InputType = GetInputType(EventToConsider);
	if (IsRelevantInput(SlateApp, EventToConsider, InputType))
	{
		if (IsInputMethodBlocked(InputType))
		{
			return true;
		}

		if (CVarShouldMouseWheelAndGestureRefreshInputMethod.GetValueOnGameThread())
		{
			RefreshCurrentInputMethod(InputType);
		}
	}*/
	return false;
}


bool FNLInputProcessor::IsInputMethodBlocked(ECommonInputType InputType) const
{
	return InputMethodPermissions[(uint8)InputType];
}