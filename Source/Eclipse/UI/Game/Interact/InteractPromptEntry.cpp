// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractPromptEntry.h"
#include "../Common/ActionCombineBox.h"
#include "CommonTextBlock.h"

#include "InputMappingContext.h"
#include "../../../Subsystems/EpInputManagerSubSystem.h"
#include "CommonUITypes.h"

void UInteractPromptEntry::Set(const FInteractActionParam& InParam)
{
	Parameter = InParam;
}


void UInteractPromptEntry::Refresh()
{
	if (!ensure(Parameter.Action)) return;

	ULocalPlayer* LocalPlayer = GetOwningLocalPlayer();
	if (!ensure(LocalPlayer))
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid Local Player! %s"), ANSI_TO_TCHAR(__FUNCTION__));
		return;
	}

	UEpInputManagerSubSystem* InputManager = LocalPlayer->GetSubsystem<UEpInputManagerSubSystem>();
	if (!ensure(InputManager))
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid Input Manager! %s"), ANSI_TO_TCHAR(__FUNCTION__));
		return;
	}

	TArray<const UInputAction*> ChordedActions;

	TArray<FKey> ChordedKeys;
	FKey ActionKey;

	if (Parameter.Context)
	{
		const TArray<FEnhancedActionKeyMapping>& Mappings = Parameter.Context->GetMappings();
		for (auto& Map : Mappings)
		{
			if (Parameter.Action != Map.Action || false == CommonUI::IsKeyValidForInputType(Map.Key, InputManager->GetInputType())) { continue; }

			ActionKey = Map.Key;
			TArray<TObjectPtr<UInputTrigger>> InputTriggers = Map.Triggers;

			for (auto& Trigger : InputTriggers)
			{
				UInputTriggerChordAction* ChordedAction = Cast<UInputTriggerChordAction>(Trigger);
				if (nullptr == ChordedAction || nullptr == ChordedAction->ChordAction) continue;

				ChordedActions.Emplace(ChordedAction->ChordAction);
			}
		}


		for (auto& ChordedAction : ChordedActions)
		{
			const FEnhancedActionKeyMapping* Mapping = Parameter.Context->GetMappings().FindByPredicate([ChordedAction](FEnhancedActionKeyMapping InMapping)
				{
					return ChordedAction == InMapping.Action;
				});

			if (Mapping)
			{
				ChordedKeys.Emplace(Mapping->Key);
			}
		}

	}

	if (nullptr != ActionCombineBox)
	{
		if (false == ChordedKeys.IsEmpty())
		{
			ActionCombineBox->SetCombinedAction(ActionKey, ChordedKeys);
		}

		else
		{
			ActionCombineBox->SetAction(ActionKey);
		}
	}



	if (nullptr != ActionDisplayText)
	{
		ActionDisplayText->SetText(Parameter.DisplayText);
	}
}