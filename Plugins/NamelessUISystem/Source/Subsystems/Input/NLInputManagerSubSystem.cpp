// Fill out your copyright notice in the Description page of Project Settings.


#include "NLInputManagerSubSystem.h"
#include "CommonInputBaseTypes.h"
#include "NLInputProcessor.h"


bool UNLInputManagerSubsystem::ShouldCreateSubsystem(UObject* Outer) const
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

void UNLInputManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	UE_LOG(LogTemp, Warning, TEXT("Initialize InputManager."));

	InputProcessor = MakeShareable<FNLInputProcessor>(new FNLInputProcessor(*this));

	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().RegisterInputPreProcessor(InputProcessor, EInputPreProcessorType::PreGame);
	}

}

void UNLInputManagerSubsystem::Deinitialize()
{
	UE_LOG(LogTemp, Warning, TEXT("Deinitialize InputManager."));

	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().UnregisterInputPreProcessor(InputProcessor);
	}

	InputProcessor.Reset();
	InputProcessor = nullptr;
}

const FName UNLInputManagerSubsystem::GetCurrentGamepadName() const
{
	return GamepadInputType;
}

void UNLInputManagerSubsystem::SetInputFilter(ECommonInputType InputType, bool bIsLock)
{
	if (InputProcessor.IsValid())
	{
		InputProcessor->SetInputFilter(InputType, bIsLock);
	}
}

void UNLInputManagerSubsystem::SetCurrentInputType(ECommonInputType InputType)
{
	if (CurrentInputType != InputType)
	{
		CurrentInputType = InputType;
		OnInputMethodChangedNative.Broadcast(CurrentInputType);
	}
}

void UNLInputManagerSubsystem::SetGamepadInputType(const FName& InGamepadType)
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

bool UNLInputManagerSubsystem::IsMobileGamepadKey(const FKey& InKey)
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

FChangedGamepadDetectedEvent& UNLInputManagerSubsystem::GetOnGamepadChangeDetected()
{
	return InputProcessor->OnGamepadChangeDetected;
}
