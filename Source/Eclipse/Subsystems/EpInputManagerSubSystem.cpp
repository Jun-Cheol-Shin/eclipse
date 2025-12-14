// Fill out your copyright notice in the Description page of Project Settings.


#include "EpInputManagerSubsystem.h"
#include "../PlayerCore/EpPlayerController.h"

void UEpInputManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UEpInputManagerSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UEpInputManagerSubsystem::PlayerControllerChanged(APlayerController* NewPlayerController)
{
	Super::PlayerControllerChanged(NewPlayerController);

}