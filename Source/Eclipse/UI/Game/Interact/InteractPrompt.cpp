// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractPrompt.h"

#include "Misc/Optional.h"
#include "Input/CommonUIInputTypes.h"
#include "Input/UIActionBindingHandle.h"

#include "../../../Subsystems/EpInputManagerSubSystem.h"
#include "../../../Actor/EpDropItemActor.h"

#include "Components/DynamicEntryBox.h"
#include "Components/Image.h"
#include "Components/WidgetSwitcher.h"


void UInteractPrompt::Set(AEpDropItemActor* InActor)
{
	DropItemActor = InActor;
}

void UInteractPrompt::OnShow()
{
	if (InputComponent.IsValid())
	{
		InputComponent->BindAction(InputActions[0], ETriggerEvent::Started, this, &UInteractPrompt::OnInteract);
	}
}

void UInteractPrompt::OnHide()
{
	DropItemActor.Reset();
	ActionHandles.Reset();
}

void UInteractPrompt::OnChangedInputDevice(ECommonInputType InType)
{
	switch (InType)
	{
	case ECommonInputType::MouseAndKeyboard:
	{
		SetKeyboardMouseUI();
	}
		break;
	case ECommonInputType::Gamepad:
	{
		SetGamepadUI();
	}
		break;

	default:
	case ECommonInputType::Touch:
	case ECommonInputType::Count:
		break;
	}

}

void UInteractPrompt::OnInteract()
{
	UE_LOG(LogTemp, Error, TEXT("Interact!!!!!!!!!"));
}

void UInteractPrompt::OnPing()
{

}

void UInteractPrompt::OnUseDirect()
{

}

void UInteractPrompt::OnHoldAction()
{

}


void UInteractPrompt::SetGamepadUI()
{
	// Show Hold
	if (nullptr != DisableImage)			{ DisableImage->SetVisibility(ESlateVisibility::Collapsed); }
	if (nullptr != ExpandSwitcher)			{ ExpandSwitcher->SetActiveWidgetIndex(0); }
	//	if (nullptr != InteractEntry)		{ InteractEntry->Set(); }

}

void UInteractPrompt::SetKeyboardMouseUI()
{
	if (nullptr != DisableImage) { DisableImage->SetVisibility(ESlateVisibility::Collapsed); }
	if (nullptr != ExpandSwitcher) { ExpandSwitcher->SetActiveWidgetIndex(1); }
	//	if (nullptr != InteractEntry)		{ InteractEntry->Set(); }
}

TOptional<FUIInputConfig> UInteractPrompt::GetDesiredInputConfig() const
{
	return FUIInputConfig(ECommonInputMode::Game, EMouseCaptureMode::CapturePermanently_IncludingInitialMouseDown, EMouseLockMode::LockOnCapture);
}
