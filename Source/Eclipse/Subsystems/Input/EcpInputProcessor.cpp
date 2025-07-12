// Fill out your copyright notice in the Description page of Project Settings.


#include "EcpInputProcessor.h"
#include "EcpInputManagerSubSystem.h"
#include "ICommonInputModule.h"

#include "Engine/Engine.h"
#include "Engine/LocalPlayer.h"
#include "Engine/GameViewportClient.h"

#include "Widgets/SViewport.h"

#include "CommonInputBaseTypes.h"

bool FEcpInputProcessor::IsRelevantInput(FSlateApplication& SlateApp, const FInputEvent& InputEvent, const ECommonInputType DesiredInputType)
{

#if WITH_EDITOR
	// 중단점에서 멈췄을 경우, 이 입력 전처리기는 들어오는 모든 입력을 무시해야 합니다.
	// 이제 편집기에서 게임 루프 외부의 작업을 수행하게 되므로, 이 모든 작업을 차단해서는 안 됩니다.
	// 대화상자를 생성하는 동안 입력을 일시 중단한 후 다른 중단점에 도달한 후
	// 편집기를 사용하려고 하면 갑자기 아무 작업도 할 수 없게 될 수 있습니다.

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
		// PIE는 특히 두 개의 클라이언트를 실행할 때 매우 특별한 기능입니다. ControllerId가 0인 두 개의 LocalPlayer가 있습니다.
		// 편집기에는 이 시나리오를 처리하기 위한 기존 기능이 있으므로, 현재는 해당 기능을 사용하고 있습니다.
		// 궁극적으로는 편집기가 SlateApplication 수준에서 사용자 지정 ISlateInputMapping 등을 사용하여 이 문제를 해결하는 것이 이상적입니다.
		GEngine->RemapGamepadControllerIdForPIE(LocalPlayer.ViewportClient, ControllerId);
#endif
		return ControllerId == InputEvent.GetUserIndex();
	}

	return false;
}

void FEcpInputProcessor::RefreshCurrentInputMethod(ECommonInputType InputMethod)
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

ECommonInputType FEcpInputProcessor::GetInputType(const FKey& InKey)
{
	// If the key is a gamepad key or if the key is a Click key (which simulates a click), we should be in Gamepad mode
	if (InKey.IsGamepadKey())
	{
		if (UEcpInputManagerSubSystem::IsMobileGamepadKey(InKey))
		{
			return ECommonInputType::Touch;
		}

		return ECommonInputType::Gamepad;
	}

	return ECommonInputType::MouseAndKeyboard;
}


ECommonInputType FEcpInputProcessor::GetInputType(const FPointerEvent& InPointerEvent)
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

bool FEcpInputProcessor::HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent)
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

bool FEcpInputProcessor::HandleAnalogInputEvent(FSlateApplication& SlateApp, const FAnalogInputEvent& InAnalogInputEvent)
{
	return false;
}

bool FEcpInputProcessor::HandleMouseMoveEvent(FSlateApplication& SlateApp, const FPointerEvent& InMouseEvent)
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

bool FEcpInputProcessor::HandleMouseButtonDownEvent(FSlateApplication& SlateApp, const FPointerEvent& InMouseEvent)
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

bool FEcpInputProcessor::HandleMouseButtonDoubleClickEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent)
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

bool FEcpInputProcessor::HandleMouseWheelOrGestureEvent(FSlateApplication& SlateApp, const FPointerEvent& InWheelEvent, const FPointerEvent* InGestureEvent)
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


bool FEcpInputProcessor::IsInputMethodBlocked(ECommonInputType InputType) const
{
	return InputMethodPermissions[(uint8)InputType];
}