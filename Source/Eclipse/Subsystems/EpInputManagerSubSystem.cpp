// Fill out your copyright notice in the Description page of Project Settings.


#include "EpInputManagerSubSystem.h"
#include "../PlayerCore/EpPlayerController.h"

void UEpInputManagerSubSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UEpInputManagerSubSystem::Deinitialize()
{
	Super::Deinitialize();
}

void UEpInputManagerSubSystem::PlayerControllerChanged(APlayerController* NewPlayerController)
{
	Super::PlayerControllerChanged(NewPlayerController);

}