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
	// �ߴ������� ������ ���, �� �Է� ��ó����� ������ ��� �Է��� �����ؾ� �մϴ�.
	// ���� �����⿡�� ���� ���� �ܺ��� �۾��� �����ϰ� �ǹǷ�, �� ��� �۾��� �����ؼ��� �� �˴ϴ�.
	// ��ȭ���ڸ� �����ϴ� ���� �Է��� �Ͻ� �ߴ��� �� �ٸ� �ߴ����� ������ ��
	// �����⸦ ����Ϸ��� �ϸ� ���ڱ� �ƹ� �۾��� �� �� ���� �� �� �ֽ��ϴ�.

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
		// PIE�� Ư�� �� ���� Ŭ���̾�Ʈ�� ������ �� �ſ� Ư���� ����Դϴ�. ControllerId�� 0�� �� ���� LocalPlayer�� �ֽ��ϴ�.
		// �����⿡�� �� �ó������� ó���ϱ� ���� ���� ����� �����Ƿ�, ����� �ش� ����� ����ϰ� �ֽ��ϴ�.
		// �ñ������δ� �����Ⱑ SlateApplication ���ؿ��� ����� ���� ISlateInputMapping ���� ����Ͽ� �� ������ �ذ��ϴ� ���� �̻����Դϴ�.
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