// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractPrompt.h"

#include "Misc/Optional.h"


#include "../../../Subsystems/EpInputManagerSubSystem.h"
#include "../../../Actor/EpDropItemActor.h"

#include "Components/DynamicEntryBox.h"
#include "Components/Image.h"
#include "Components/WidgetSwitcher.h"

#include "../../../Subsystems/EpGameDataSubSystem.h"

void UInteractPrompt::SetItem(int32 InItemId)
{
	ItemId = InItemId;
}

void UInteractPrompt::OnShow()
{
	BindUIAction(TEXT("IA_UI_Interact"), ETriggerEvent::Started, this, TEXT("OnInteract"));
}

void UInteractPrompt::OnHide()
{

	//DropItemActor.Reset();
}

void UInteractPrompt::OnChangedInputDevice(ECommonInputType InType)
{
	switch (InType)
	{
	case ECommonInputType::MouseAndKeyboard:
	{
		SetKeyboardMouseUI();
		BindUIAction(TEXT("IA_UI_Ping"), ETriggerEvent::Started, this, TEXT("OnPing"));

		// TODO : is possible use direct item?

	}
		break;
	case ECommonInputType::Gamepad:
	{
		SetGamepadUI();

		if (0 != ItemId)
		{
			if (nullptr != PingEntry) { PingEntry->Refresh(); }

			if (nullptr == GetGameInstance()) { return; }

			UEpGameDataSubSystem* GameDataManager = GetGameInstance()->GetSubsystem<UEpGameDataSubSystem>();
			if (nullptr == GameDataManager) { return; }

			const FItemDataRow* ItemData = GameDataManager->GetGameData<FItemDataRow>(ItemId);
			if (ensure(ItemData))
			{
				// TODO : check consumable item?
				//		BindUIAction(TEXT("IA_UI_Hold"), ETriggerEvent::Started, this, TEXT("OnHoldAction"));

			}
		}

		BindUIAction(TEXT("IA_UI_Hold"), ETriggerEvent::Completed, this, TEXT("OnHoldAction"));
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

	OnInteractDelegate.Broadcast();
}

void UInteractPrompt::OnPing()
{
	UE_LOG(LogTemp, Error, TEXT("Ping!!!!!!!!!"));

	OnPingDelegate.Broadcast();
}

void UInteractPrompt::OnUseDirect()
{
	UE_LOG(LogTemp, Error, TEXT("Use Direct!!!!!!!!!"));

}

void UInteractPrompt::OnHoldAction()
{
	RemoveUIAction(TEXT("IA_UI_Interact"));

	SetKeyboardMouseUI();
	if (nullptr != DisableImage)			{ DisableImage->SetVisibility(ESlateVisibility::HitTestInvisible); }

	BindUIAction(TEXT("IA_UI_Ping"), ETriggerEvent::Started, this, TEXT("OnPing"));
}


void UInteractPrompt::SetGamepadUI()
{
	// Show Hold
	if (nullptr != DisableImage)			{ DisableImage->SetVisibility(ESlateVisibility::Collapsed); }
	if (nullptr != ExpandSwitcher)			{ ExpandSwitcher->SetActiveWidgetIndex(0); }
	if (nullptr != InteractEntry)			{ InteractEntry->Refresh(); }
}

void UInteractPrompt::SetKeyboardMouseUI()
{
	if (nullptr != DisableImage)			{ DisableImage->SetVisibility(ESlateVisibility::Collapsed); }
	if (nullptr != ExpandSwitcher)			
	{
		ExpandSwitcher->SetVisibility(0 != ItemId ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
		ExpandSwitcher->SetActiveWidgetIndex(1); 
	}

	if (nullptr != InteractEntry)			{ InteractEntry->Refresh(); }

	if (0 != ItemId)
	{
		if (nullptr != PingEntry) { PingEntry->Refresh(); }

		if (nullptr == GetGameInstance())	{ return; }

		UEpGameDataSubSystem* GameDataManager = GetGameInstance()->GetSubsystem<UEpGameDataSubSystem>();
		if (nullptr == GameDataManager)		{ return; }

		const FItemDataRow* ItemData = GameDataManager->GetGameData<FItemDataRow>(ItemId);
		if (ensure(ItemData))
		{
			// TODO : check consumable item?
			


			//BindUIAction(TEXT("IA_UI_UseDirect"), ETriggerEvent::Started, this, TEXT("OnUseDirect"));
		}
	}
}

TOptional<FUIInputConfig> UInteractPrompt::GetDesiredInputConfig() const
{
	return FUIInputConfig(ECommonInputMode::Game, EMouseCaptureMode::CapturePermanently_IncludingInitialMouseDown, EMouseLockMode::LockOnCapture);
}
