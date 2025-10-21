// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionCombineBox.h"
#include "ActionCombineEntry.h"

#include "Components/DynamicEntryBox.h"
#include "../../../Subsystems/EpInputManagerSubSystem.h"

void UActionCombineBox::SetAction(const UInputAction* InputAction)
{
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

	TArray<FKey> Keys = InputManager->QueryKeysMappedToAction(InputAction);
	const ECommonInputType CurType = InputManager->GetInputType();


	Keys.RemoveAll([CurType](const FKey& InKey)
		{ 
			switch (CurType)
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



	if (nullptr != EntryBox)
	{
		EntryBox->Reset();
		for (int i = 0; i < Keys.Num(); ++i)
		{
			UActionCombineEntry* NewEntry = EntryBox->CreateEntry<UActionCombineEntry>();
			
			if (!ensure(NewEntry))
			{
				continue;
			}

			NewEntry->SetKey(Keys[i]);
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
