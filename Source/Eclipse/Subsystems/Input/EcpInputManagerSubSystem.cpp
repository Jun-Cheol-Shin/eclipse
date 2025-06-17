// Fill out your copyright notice in the Description page of Project Settings.


#include "EcpInputManagerSubSystem.h"
#include "EcpInputProcessor.h"


void UEcpInputManagerSubSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	UE_LOG(LogTemp, Warning, TEXT("Initialize InputManager."));

	InputProcessor = MakeShareable<FEcpInputProcessor>(new FEcpInputProcessor());

	FSlateApplication::Get().RegisterInputPreProcessor(InputProcessor);

}

void UEcpInputManagerSubSystem::Deinitialize()
{
	UE_LOG(LogTemp, Warning, TEXT("Deinitialize InputManager."));

	FSlateApplication::Get().UnregisterInputPreProcessor(InputProcessor);

	InputProcessor.Reset();
	InputProcessor = nullptr;
}
