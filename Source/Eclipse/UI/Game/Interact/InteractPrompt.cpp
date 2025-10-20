// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractPrompt.h"
#include "Components/DynamicEntryBox.h"
#include "Misc/Optional.h"

void UInteractPrompt::Set(const TArray<FInteractActionParam>& InParams)
{
	if (nullptr != EntryBox)
	{
		EntryBox->Reset();

		for (int i = 0; i < InParams.Num(); ++i)
		{
			UInteractPromptEntry* NewEntry = EntryBox->CreateEntry<UInteractPromptEntry>();
			if (!ensure(NewEntry)) continue;

			NewEntry->Set(InParams[i]);
		}
	}
}

void UInteractPrompt::NativeOnActivated()
{
	Super::NativeOnActivated();

	if (nullptr != EntryBox)
	{
		EntryBox->SetVisibility(ESlateVisibility::HitTestInvisible);
	}

}

void UInteractPrompt::NativeOnDeactivated()
{



	Super::NativeOnDeactivated();
}

TOptional<FUIInputConfig> UInteractPrompt::GetDesiredInputConfig() const
{
	return FUIInputConfig(ECommonInputMode::Game, EMouseCaptureMode::CapturePermanently_IncludingInitialMouseDown, EMouseLockMode::LockOnCapture);
}
