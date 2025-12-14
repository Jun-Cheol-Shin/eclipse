// Fill out your copyright notice in the Description page of Project Settings.


#include "EclipseFunctionLibrary.h"
#include "GameModes/EpGameInstance.h"

UEpGameDataSubsystem* UEclipseFunctionLibrary::GetGameDataSubSytem(UWorld* InWorld)
{
	if (!ensure(InWorld))
	{
		return nullptr;
	}

	UEpGameInstance* GameInst = Cast<UEpGameInstance>(InWorld->GetGameInstance());
	if (!ensure(GameInst))
	{
		return nullptr;
	}

	return GameInst->GetSubsystem<UEpGameDataSubsystem>();
}

UEpInputManagerSubsystem* UEclipseFunctionLibrary::GetInputManagerSubSytem(ULocalPlayer* InLocalPlayer)
{
	if (nullptr == InLocalPlayer)
	{
		return nullptr;
	}

	return ULocalPlayer::GetSubsystem<UEpInputManagerSubsystem>(InLocalPlayer);
}

UEpResourceSubsystem* UEclipseFunctionLibrary::GetResourceSubSytem(UWorld* InWorld)
{
	if (!ensure(InWorld))
	{
		return nullptr;
	}

	UEpGameInstance* GameInst = Cast<UEpGameInstance>(InWorld->GetGameInstance());
	if (!ensure(GameInst))
	{
		return nullptr;
	}

	return GameInst->GetSubsystem<UEpResourceSubsystem>();
}

UEpUIManagerSubsystem* UEclipseFunctionLibrary::GetUIManagerSubSystem(UWorld* InWorld)
{
	if (!ensure(InWorld))
	{
		return nullptr;
	}

	UEpGameInstance* GameInst = Cast<UEpGameInstance>(InWorld->GetGameInstance());
	if (!ensure(GameInst))
	{
		return nullptr;
	}

	return GameInst->GetSubsystem<UEpUIManagerSubsystem>();
}
