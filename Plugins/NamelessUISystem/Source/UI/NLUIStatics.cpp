// Fill out your copyright notice in the Description page of Project Settings.


#include "NLUIStatics.h"

#include "../NLGameInstance.h"
#include "../Subsystems/Input/NLInputManagerSubSystem.h"
#include "../Subsystems/NLUIManagerSubsystem.h"

void UNLUIStatics::ShowLayerWidget(const ULocalPlayer* InLocalPlayer, const FGameplayTag& InLayer, TSoftClassPtr<UCommonActivatableWidget> InWidgetClass)
{
	if (nullptr == InLocalPlayer) return;

	UNLGameInstance* EcpGameInstance = Cast<UNLGameInstance>(InLocalPlayer->GetGameInstance());
	if (false == ensure(EcpGameInstance)) return;

	UNLUIManagerSubsystem* UISubSystem = EcpGameInstance->GetSubsystem<UNLUIManagerSubsystem>();
	if (nullptr != UISubSystem)
	{
		UISubSystem->ShowLayerWidget(InLayer, InWidgetClass);
	}
}

void UNLUIStatics::ShowLayerWidgetFromPlayerController(const APlayerController* InController, const FGameplayTag& InLayerType, TSoftClassPtr<UCommonActivatableWidget> InWidgetClass)
{
	ShowLayerWidget(InController->GetLocalPlayer(), InLayerType, InWidgetClass);
}

void UNLUIStatics::HideLayerWidget(const ULocalPlayer* InLocalPlayer, UCommonActivatableWidget* InLayerWidget)
{
	if (nullptr == InLocalPlayer) return;

	UNLGameInstance* EcpGameInstance = Cast<UNLGameInstance>(InLocalPlayer->GetGameInstance());
	if (false == ensure(EcpGameInstance)) return;

	UNLUIManagerSubsystem* UISubSystem = EcpGameInstance->GetSubsystem<UNLUIManagerSubsystem>();
	if (nullptr != UISubSystem)
	{
		UISubSystem->HideLayerWidget(InLayerWidget);
	}
}

void UNLUIStatics::HideLayerWidgetFromPlayerController(const APlayerController* InController, UCommonActivatableWidget* InLayerWidget)
{
	HideLayerWidget(InController->GetLocalPlayer(), InLayerWidget);
}

void UNLUIStatics::SetInputLockFromPlayerController(const APlayerController* InController, bool bLock)
{
	if (nullptr == InController) return;

	ULocalPlayer* MyLocalPlayer = InController->GetLocalPlayer();
	if (nullptr == MyLocalPlayer) return;

	SetInputLock(MyLocalPlayer, bLock);
}

void UNLUIStatics::SetInputLock(const ULocalPlayer* InLocalPlayer, bool bLock)
{
	if (nullptr == InLocalPlayer) return;

	UNLInputManagerSubSystem* InputSubSystem = InLocalPlayer->GetSubsystem<UNLInputManagerSubSystem>();
	if (nullptr != InputSubSystem)
	{
		InputSubSystem->SetInputFilter(ECommonInputType::MouseAndKeyboard, bLock);
		InputSubSystem->SetInputFilter(ECommonInputType::Gamepad, bLock);
		InputSubSystem->SetInputFilter(ECommonInputType::Touch, bLock);
	}
}
