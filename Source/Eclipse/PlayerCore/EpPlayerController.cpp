// Fill out your copyright notice in the Description page of Project Settings.


#include "EpPlayerController.h"
#include "Engine/LocalPlayer.h"

#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "../Option/EpInputConfig.h"

#include "../Subsystems/EpInputManagerSubSystem.h"
#include "../Subsystems/EpUIManagerSubsystem.h"

void AEpPlayerController::OnInventory()
{
	UE_LOG(LogTemp, Error, TEXT("Show Inventory!"));
}

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


	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	if (!ensure(EnhancedInputComponent))
	{
		return;
	}


	// Bind Action..
	if (const UInputAction* InventoryAction = EclipseInputConfig->FindNativeInputActionForTag(
		FGameplayTag::RequestGameplayTag(FName("InputTag.Inventory"), true)))
	{
		EnhancedInputComponent->BindAction(InventoryAction, ETriggerEvent::Started, this, &AEpPlayerController::OnInventory);
	}


}
