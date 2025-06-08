// Fill out your copyright notice in the Description page of Project Settings.

#include "EcpUIManagerSubsystem.h"

bool UEcpUIManagerSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
    if (!Super::ShouldCreateSubsystem(Outer))
    {
        return false;
    }

    // only possible create from server?

    UWorld* World = Cast<UWorld>(Outer);
    return World->GetNetMode() < NM_Client;
}

void UEcpUIManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	UE_LOG(LogTemp, Warning, TEXT("Initialize UIManager."));
}

void UEcpUIManagerSubsystem::Deinitialize()
{
	UE_LOG(LogTemp, Warning, TEXT("Deinitialize UIManager."));
}
