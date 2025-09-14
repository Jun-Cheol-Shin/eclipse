// Fill out your copyright notice in the Description page of Project Settings.


#include "EpPlayerController.h"
#include "../Subsystems/EpInputManagerSubSystem.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"

void AEpPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// Add Input Mapping Contexts
	if (UEpInputManagerSubSystem* Subsystem = ULocalPlayer::GetSubsystem<UEpInputManagerSubSystem>(GetLocalPlayer()))
	{
		Subsystem->ClearAllMappings();

		for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
		{
			Subsystem->AddMappingContext(CurrentContext, 0);
		}
	}
}
