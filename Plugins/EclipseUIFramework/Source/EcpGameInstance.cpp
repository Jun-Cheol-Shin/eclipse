// Fill out your copyright notice in the Description page of Project Settings.


#include "EcpGameInstance.h"
#include "Subsystems/EcpUIManagerSubsystem.h"



void UEcpGameInstance::Init()
{
	Super::Init();

	OnLocalPlayerAddedEvent.AddUObject(this, &UEcpGameInstance::OnAddedLocalPlayer);
	OnLocalPlayerRemovedEvent.AddUObject(this, &UEcpGameInstance::OnRemoveLocalPlayer);
}

void UEcpGameInstance::Shutdown()
{
	OnLocalPlayerRemovedEvent.RemoveAll(this);
	OnLocalPlayerAddedEvent.RemoveAll(this);

	Super::Shutdown();
}

void UEcpGameInstance::OnAddedLocalPlayer(ULocalPlayer* InNewPlayer)
{
	UEcpUIManagerSubsystem* UISubSystem = GetSubsystem<UEcpUIManagerSubsystem>();
	if (ensure(UISubSystem))
	{
		UISubSystem->NotifyPlayerAdded(InNewPlayer);
	}
}

void UEcpGameInstance::OnRemoveLocalPlayer(ULocalPlayer* InRemovedPlayer)
{
	UEcpUIManagerSubsystem* UISubSystem = GetSubsystem<UEcpUIManagerSubsystem>();
	if (ensure(UISubSystem))
	{
		UISubSystem->NotifyPlayerRemoved(InRemovedPlayer);
	}
}
