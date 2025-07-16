// Fill out your copyright notice in the Description page of Project Settings.


#include "NLGameInstance.h"
#include "Subsystems/NLUIManagerSubsystem.h"



void UNLGameInstance::Init()
{
	Super::Init();

	OnLocalPlayerAddedEvent.AddUObject(this, &UNLGameInstance::OnAddedLocalPlayer);
	OnLocalPlayerRemovedEvent.AddUObject(this, &UNLGameInstance::OnRemoveLocalPlayer);
}

void UNLGameInstance::Shutdown()
{
	OnLocalPlayerRemovedEvent.RemoveAll(this);
	OnLocalPlayerAddedEvent.RemoveAll(this);

	Super::Shutdown();
}

void UNLGameInstance::OnAddedLocalPlayer(ULocalPlayer* InNewPlayer)
{
	UNLUIManagerSubsystem* UISubSystem = GetSubsystem<UNLUIManagerSubsystem>();
	if (ensure(UISubSystem))
	{
		UISubSystem->NotifyPlayerAdded(InNewPlayer);
	}
}

void UNLGameInstance::OnRemoveLocalPlayer(ULocalPlayer* InRemovedPlayer)
{
	UNLUIManagerSubsystem* UISubSystem = GetSubsystem<UNLUIManagerSubsystem>();
	if (ensure(UISubSystem))
	{
		UISubSystem->NotifyPlayerRemoved(InRemovedPlayer);
	}
}
