// Fill out your copyright notice in the Description page of Project Settings.


#include "EcpInputManagerSubSystem.h"
#include "CommonInputBaseTypes.h"
#include "EcpInputProcessor.h"


void UEcpInputManagerSubSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	UE_LOG(LogTemp, Warning, TEXT("Initialize InputManager."));

	InputProcessor = MakeShareable<FEcpInputProcessor>(new FEcpInputProcessor(*this));

	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().RegisterInputPreProcessor(InputProcessor, EInputPreProcessorType::PreGame);
	}

}

void UEcpInputManagerSubSystem::Deinitialize()
{
	UE_LOG(LogTemp, Warning, TEXT("Deinitialize InputManager."));

	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().UnregisterInputPreProcessor(InputProcessor);
	}

	InputProcessor.Reset();
	InputProcessor = nullptr;
}

const FName UEcpInputManagerSubSystem::GetCurrentGamepadName() const
{
	return GamepadInputType;
}

void UEcpInputManagerSubSystem::SetInputFilter(ECommonInputType InputType, bool bIsLock)
{
	if (InputProcessor.IsValid())
	{
		InputProcessor->SetInputFilter(InputType, bIsLock);
	}
}

void UEcpInputManagerSubSystem::SetCurrentInputType(ECommonInputType InputType)
{
	CurrentInputType = InputType;
}

void UEcpInputManagerSubSystem::SetGamepadInputType(const FName& InGamepadType)
{
	if (ensure(UCommonInputPlatformSettings::Get()->CanChangeGamepadType()))
	{
		GamepadInputType = InGamepadType;

		if (UWorld* World = GetWorld())
		{
			if (!World->bIsTearingDown)
			{
				OnInputMethodChangedNative.Broadcast(CurrentInputType);
			}
		}
	}

	GamepadInputType = InGamepadType;
}

bool UEcpInputManagerSubSystem::IsMobileGamepadKey(const FKey& InKey)
{
	// Mobile keys that can be physically present on the device
	static TArray<FKey> PhysicalMobileKeys = {
		EKeys::Android_Back,
		EKeys::Android_Menu,
		EKeys::Android_Volume_Down,
		EKeys::Android_Volume_Up
	};

	return PhysicalMobileKeys.Contains(InKey);
}

FChangedGamepadDetectedEvent& UEcpInputManagerSubSystem::GetOnGamepadChangeDetected()
{
	return InputProcessor->OnGamepadChangeDetected;
}

FChangedInputTypeDetectedEvent& UEcpInputManagerSubSystem::GetOnTouchDetected()
{
	return InputProcessor->OnTouchChangeDetected;
}

FChangedInputTypeDetectedEvent& UEcpInputManagerSubSystem::GetOnMouseAndKeyboardDetected()
{
	return InputProcessor->OnMouseAndKeyboardChangeDetected;
}