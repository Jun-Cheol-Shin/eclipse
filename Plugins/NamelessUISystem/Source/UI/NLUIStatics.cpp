// Fill out your copyright notice in the Description page of Project Settings.


#include "NLUIStatics.h"

#include "../NLGameInstance.h"
#include "../Subsystems/Input/NLInputManagerSubsystem.h"
#include "../Subsystems/NLUIManagerSubsystem.h"

UNLGameLayout* UNLUIStatics::GetGameLayoutForPlayer(const ULocalPlayer* InLocalPlayer)
{
	UNLGameInstance* EcpGameInstance = Cast<UNLGameInstance>(InLocalPlayer->GetGameInstance());
	if (false == ensure(EcpGameInstance)) return nullptr;

	UNLUIManagerSubsystem* UISubSystem = EcpGameInstance->GetSubsystem<UNLUIManagerSubsystem>();
	if (false == ensure(UISubSystem)) return nullptr;

	return UISubSystem->GetPlayerGameLayout(InLocalPlayer);
}

UCommonActivatableWidget* UNLUIStatics::GetTopWidget(const ULocalPlayer* InLocalPlayer, const FGameplayTag& InLayerType)
{
	UNLGameInstance* EcpGameInstance = Cast<UNLGameInstance>(InLocalPlayer->GetGameInstance());
	if (false == ensure(EcpGameInstance)) return nullptr;

	UNLUIManagerSubsystem* UISubSystem = EcpGameInstance->GetSubsystem<UNLUIManagerSubsystem>();
	if (false == ensure(UISubSystem)) return nullptr;

	return UISubSystem->GetTopLayerWidget(InLayerType);
}

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

void UNLUIStatics::HideLayerWidget(const ULocalPlayer* InLocalPlayer, const FGameplayTag& InLayerType, UCommonActivatableWidget* InLayerWidget)
{
	if (nullptr == InLocalPlayer) return;

	UNLGameInstance* EcpGameInstance = Cast<UNLGameInstance>(InLocalPlayer->GetGameInstance());
	if (false == ensure(EcpGameInstance)) return;

	UNLUIManagerSubsystem* UISubSystem = EcpGameInstance->GetSubsystem<UNLUIManagerSubsystem>();
	if (nullptr != UISubSystem)
	{
		UISubSystem->HideLayerWidget(InLayerType, InLayerWidget);
	}
}

void UNLUIStatics::HideLayerWidgetFromPlayerController(const APlayerController* InController, const FGameplayTag& InLayerType, UCommonActivatableWidget* InLayerWidget)
{
	HideLayerWidget(InController->GetLocalPlayer(), InLayerType, InLayerWidget);
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

	UNLInputManagerSubsystem* InputSubSystem = InLocalPlayer->GetSubsystem<UNLInputManagerSubsystem>();
	if (nullptr != InputSubSystem)
	{
		InputSubSystem->SetInputFilter(ECommonInputType::MouseAndKeyboard, bLock);
		InputSubSystem->SetInputFilter(ECommonInputType::Gamepad, bLock);
		InputSubSystem->SetInputFilter(ECommonInputType::Touch, bLock);
	}
}
