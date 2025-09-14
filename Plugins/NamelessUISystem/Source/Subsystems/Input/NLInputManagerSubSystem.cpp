// Fill out your copyright notice in the Description page of Project Settings.


#include "NLInputManagerSubSystem.h"
#include "CommonInputBaseTypes.h"
#include "NLInputProcessor.h"


bool UNLInputManagerSubSystem::ShouldCreateSubsystem(UObject* Outer) const
{
#if WITH_EDITOR
	return true;
#else
	if (!CastChecked<UGameInstance>(Outer)->IsDedicatedServerInstance())
	{
		TArray<UClass*> ChildClasses;
		GetDerivedClasses(GetClass(), ChildClasses, false);

		// Only create an instance if there is no override implementation defined elsewhere
		return ChildClasses.Num() == 0;
	}

	return false;
#endif
}

void UNLInputManagerSubSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	UE_LOG(LogTemp, Warning, TEXT("Initialize InputManager."));

	InputProcessor = MakeShareable<FNLInputProcessor>(new FNLInputProcessor(*this));

	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().RegisterInputPreProcessor(InputProcessor, EInputPreProcessorType::PreGame);
	}

}

void UNLInputManagerSubSystem::Deinitialize()
{
	UE_LOG(LogTemp, Warning, TEXT("Deinitialize InputManager."));

	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().UnregisterInputPreProcessor(InputProcessor);
	}

	InputProcessor.Reset();
	InputProcessor = nullptr;
}

const FName UNLInputManagerSubSystem::GetCurrentGamepadName() const
{
	return GamepadInputType;
}

void UNLInputManagerSubSystem::SetInputFilter(ECommonInputType InputType, bool bIsLock)
{
	if (InputProcessor.IsValid())
	{
		InputProcessor->SetInputFilter(InputType, bIsLock);
	}
}

void UNLInputManagerSubSystem::SetCurrentInputType(ECommonInputType InputType)
{
	if (CurrentInputType != InputType)
	{
		switch (InputType)
		{
		case ECommonInputType::MouseAndKeyboard:
			OnChangedDetectMouseAndKeyboard.Broadcast();
			break;
		case ECommonInputType::Gamepad:
			OnChangedDetectGamePad.Broadcast();
			break;
		case ECommonInputType::Touch:
			OnChangedDetectTouch.Broadcast();
			break;

		default:		
		case ECommonInputType::Count:
			break;
		}
	}

	CurrentInputType = InputType;
}

void UNLInputManagerSubSystem::SetGamepadInputType(const FName& InGamepadType)
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

bool UNLInputManagerSubSystem::IsMobileGamepadKey(const FKey& InKey)
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

FChangedGamepadDetectedEvent& UNLInputManagerSubSystem::GetOnGamepadChangeDetected()
{
	return InputProcessor->OnGamepadChangeDetected;
}
