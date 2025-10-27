// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionCombineBox.h"
#include "ActionCombineEntry.h"

#include "Components/DynamicEntryBox.h"

void UActionCombineBox::SetAction(const FKey& InKey)
{	
	Refresh({ InKey });
}

void UActionCombineBox::SetCombinedAction(const FKey& InKey, const TArray<FKey>& InChordedKeys)
{
	Add(InChordedKeys);
	Add({ InKey });
}

void UActionCombineBox::Add(const TArray<FKey>& InKeys)
{
	/*TArray<FKey> FilteredKeys = InKeys.FilterByPredicate([InputType](const FKey& InKey)
		{
			switch (InputType)
			{
			case ECommonInputType::MouseAndKeyboard:
			{
				return InKey.IsTouch() || InKey.IsGamepadKey();
			}
			break;

			case ECommonInputType::Gamepad:
			{
				return false == InKey.IsGamepadKey();
			}
			break;

			case ECommonInputType::Touch:
			{
				return false == InKey.IsTouch();
			}
			break;

			default:
				break;
			}

			return true;
		});
	*/

	if (nullptr != EntryBox)
	{
		for (int i = 0; i < InKeys.Num(); ++i)
		{
			UActionCombineEntry* NewEntry = EntryBox->CreateEntry<UActionCombineEntry>();

			if (!ensure(NewEntry))
			{
				continue;
			}

			NewEntry->SetKey(InKeys[i]);
			NewEntry->SetEnableSign(EntryBox->GetAllEntries().Num() > 1);
		}

		EntryBox->SetVisibility(EntryBox->GetAllEntries().IsEmpty() ? ESlateVisibility::Collapsed : ESlateVisibility::SelfHitTestInvisible);
	}
}

void UActionCombineBox::Refresh(const TArray<FKey>& InKeys)
{
	/* TArray<FKey> FilteredKeys = InKeys.FilterByPredicate([InputType](const FKey& InKey)
		{
			
		});
	*/

	if (nullptr != EntryBox)
	{
		EntryBox->Reset();
		for (int i = 0; i < InKeys.Num(); ++i)
		{
			UActionCombineEntry* NewEntry = EntryBox->CreateEntry<UActionCombineEntry>();

			if (!ensure(NewEntry))
			{
				continue;
			}

			NewEntry->SetKey(InKeys[i]);
			NewEntry->SetEnableSign(i > 0);
		}


		EntryBox->SetVisibility(EntryBox->GetAllEntries().IsEmpty() ? ESlateVisibility::Collapsed : ESlateVisibility::SelfHitTestInvisible);
	}
}

void UActionCombineBox::NativeConstruct()
{
	Super::NativeConstruct();

	if (nullptr != EntryBox)
	{
		EntryBox->Reset();
	}
}
