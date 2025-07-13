// Fill out your copyright notice in the Description page of Project Settings.


#include "EcpUIFunctions.h"

#include "../EcpGameInstance.h"
#include "../Subsystems/Input/EcpInputManagerSubSystem.h"
#include "../Subsystems/EcpUIManagerSubsystem.h"

void UEcpUIFunctions::ShowLayerWidget(const ULocalPlayer* InLocalPlayer, EEclipseGameLayer InLayer, TSoftClassPtr<UCommonActivatableWidget> InWidgetClass)
{
	if (nullptr == InLocalPlayer) return;

	UEcpGameInstance* EcpGameInstance = Cast<UEcpGameInstance>(InLocalPlayer->GetGameInstance());
	if (false == ensure(EcpGameInstance)) return;

	UEcpUIManagerSubsystem* UISubSystem = EcpGameInstance->GetSubsystem<UEcpUIManagerSubsystem>();
	if (nullptr != UISubSystem)
	{
		UISubSystem->ShowLayerWidget(InLayer, InWidgetClass);
	}
}

void UEcpUIFunctions::ShowLayerWidgetFromPlayerController(const APlayerController* InController, EEclipseGameLayer InLayerType, TSoftClassPtr<UCommonActivatableWidget> InWidgetClass)
{
	ShowLayerWidget(InController->GetLocalPlayer(), InLayerType, InWidgetClass);
}

void UEcpUIFunctions::HideLayerWidget(const ULocalPlayer* InLocalPlayer, UCommonActivatableWidget* InLayerWidget)
{
	if (nullptr == InLocalPlayer) return;

	UEcpGameInstance* EcpGameInstance = Cast<UEcpGameInstance>(InLocalPlayer->GetGameInstance());
	if (false == ensure(EcpGameInstance)) return;

	UEcpUIManagerSubsystem* UISubSystem = EcpGameInstance->GetSubsystem<UEcpUIManagerSubsystem>();
	if (nullptr != UISubSystem)
	{
		UISubSystem->HideLayerWidget(InLayerWidget);
	}
}

void UEcpUIFunctions::HideLayerWidgetFromPlayerController(const APlayerController* InController, UCommonActivatableWidget* InLayerWidget)
{
	HideLayerWidget(InController->GetLocalPlayer(), InLayerWidget);
}

void UEcpUIFunctions::SetInputLockFromPlayerController(const APlayerController* InController, bool bLock)
{
	if (nullptr == InController) return;

	ULocalPlayer* MyLocalPlayer = InController->GetLocalPlayer();
	if (nullptr == MyLocalPlayer) return;

	SetInputLock(MyLocalPlayer, bLock);
}

void UEcpUIFunctions::SetInputLock(const ULocalPlayer* InLocalPlayer, bool bLock)
{
	if (nullptr == InLocalPlayer) return;

	UEcpInputManagerSubSystem* InputSubSystem = InLocalPlayer->GetSubsystem<UEcpInputManagerSubSystem>();
	if (nullptr != InputSubSystem)
	{
		InputSubSystem->SetInputFilter(ECommonInputType::MouseAndKeyboard, bLock);
		InputSubSystem->SetInputFilter(ECommonInputType::Gamepad, bLock);
		InputSubSystem->SetInputFilter(ECommonInputType::Touch, bLock);
	}
}
