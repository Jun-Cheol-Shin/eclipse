// Fill out your copyright notice in the Description page of Project Settings.


#include "EcpInputManagerSubSystem.h"

bool UEcpInputManagerSubSystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return false;
}

void UEcpInputManagerSubSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	UE_LOG(LogTemp, Warning, TEXT("Initialize InputManager."));
}

void UEcpInputManagerSubSystem::Deinitialize()
{
	UE_LOG(LogTemp, Warning, TEXT("Deinitialize InputManager."));
}
