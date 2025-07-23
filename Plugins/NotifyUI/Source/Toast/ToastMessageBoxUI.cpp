// Fill out your copyright notice in the Description page of Project Settings.


#include "ToastMessageBoxUI.h"
#include "ToastMessageTextUI.h"
#include "Components/DynamicEntryBox.h"

void UToastMessageBoxUI::RegistToastInt(FGameplayTag InTag, const FString& InFormat, const TArray<int64>& InArguments)
{
	RegistToastInternal(InTag, FTextFormat::FromString(InFormat), FFormatOrderedArguments(InArguments));
}

void UToastMessageBoxUI::RegistToastDouble(FGameplayTag InTag, const FString& InFormat, const TArray<double>& InArguments)
{
	RegistToastInternal(InTag, FTextFormat::FromString(InFormat), FFormatOrderedArguments(InArguments));
}

void UToastMessageBoxUI::RegistToastFloat(FGameplayTag InTag, const FString& InFormat, const TArray<float>& InArguments)
{
	RegistToastInternal(InTag, FTextFormat::FromString(InFormat), FFormatOrderedArguments(InArguments));
}

void UToastMessageBoxUI::RegistToastUInt(FGameplayTag InTag, const FString& InFormat, const TArray<uint8>& InArguments)
{
	RegistToastInternal(InTag, FTextFormat::FromString(InFormat), FFormatOrderedArguments(InArguments));
}

void UToastMessageBoxUI::RegistToastText(FGameplayTag InTag, const FText& InText)
{
	RegistToastInternal(InTag, InText);
}

void UToastMessageBoxUI::RegistToastInternal(FGameplayTag InTag, const FText& InText)
{
	if (nullptr == ToastBox) return;

	WaitQueue.Enqueue(FToastMessageData(InTag, InText));

	if (ToastBoxMaximumCount <= ToastBox->GetAllEntries().Num())
	{
		RemoveToastTop();
	}

	else
	{
		AddToast();
	}

}

void UToastMessageBoxUI::RegistToastInternal(FGameplayTag InTag, const FTextFormat& InFormat, FFormatOrderedArguments InArguments)
{
	if (nullptr == ToastBox) return;

	FToastMessageData NewData(InTag, InFormat, InArguments);

	if (true == bMerge)
	{
		if (nullptr != NeedMergeData.Key && true == NeedMergeData.Key->IsPossibleMerge(NewData))
		{
			// merge...
			NeedMergeData.Key->Merge(NewData);
		}

		else
		{
			// reset wait data...
			WaitQueue.Enqueue(*NeedMergeData.Key);
			NeedMergeData = { &NewData, FPlatformTime::Seconds() + ToastMergeInterval };
		}
	}

	else
	{
		WaitQueue.Enqueue(NewData);

		if (ToastBoxMaximumCount <= ToastBox->GetAllEntries().Num())
		{
			RemoveToastTop();
		}

		else
		{
			AddToast();
		}
	}
}

void UToastMessageBoxUI::AddToast()
{
	if (nullptr == WaitQueue.Peek()) return;

	FToastMessageData NewData;
	WaitQueue.Dequeue(OUT NewData);

	UToastMessageTextUI* MessageUI = ToastBox->CreateEntry<UToastMessageTextUI>();
	if (nullptr != MessageUI)
	{
		MessageQueue.Emplace(NewData.MessageType, FPlatformTime::Seconds() + ToastDuration);

		MessageUI->OnCompleteAnimEndDelegate.BindUObject(this, &UToastMessageBoxUI::OnEndedFadeOutAnimation);
		MessageUI->Start(NewData, EToastStackType::Down == ToastStackType);
	}
}

void UToastMessageBoxUI::RemoveToast(const FGameplayTag& InTag)
{
	if (nullptr == ToastBox) return;

	const TArray<UToastMessageTextUI*>& EntryList = ToastBox->GetTypedEntries<UToastMessageTextUI>();

	UToastMessageTextUI* const* TextUI = EntryList.FindByPredicate([&InTag](UToastMessageTextUI* InTextUI)
		{
			return true == InTextUI->IsEqual(InTag);
		});

	if (TextUI && *TextUI)
	{
		(*TextUI)->End();
	}
}

void UToastMessageBoxUI::RemoveToastTop()
{
	const TArray<UToastMessageTextUI*>& EntryList = ToastBox->GetTypedEntries<UToastMessageTextUI>();

	if (EntryList.IsValidIndex(0))
	{
		UToastMessageTextUI* MessageUI = EntryList[0];

		if (nullptr != MessageUI)
		{
			MessageUI->End();
		}
	}
}

void UToastMessageBoxUI::NativeTick(const FGeometry& InGeometry, float InDeltaTime)
{
	Super::NativeTick(InGeometry, InDeltaTime);


	if (nullptr != NeedMergeData.Key && NeedMergeData.Value <= FPlatformTime::Seconds())
	{
		WaitQueue.Enqueue(*NeedMergeData.Key);
		NeedMergeData = { nullptr, 0.f };
		if (ToastBoxMaximumCount <= ToastBox->GetAllEntries().Num())
		{
			RemoveToastTop();
		}

		else
		{
			AddToast();
		}
	}

	for (auto& Message : MessageQueue)
	{
		if (Message.Value <= FPlatformTime::Seconds())
		{
			RemoveToast(Message.Key);
		}
	}
}

void UToastMessageBoxUI::OnEndedFadeOutAnimation(UToastMessageTextUI* InMessageText, const FGameplayTag& InMessageType)
{
	if (nullptr == ToastBox) return;

	if (ensure(MessageQueue.Contains(InMessageType)))
	{
		MessageQueue.Remove(InMessageType);
		ToastBox->RemoveEntry(InMessageText);

		OnFinishMessageSignature.Broadcast(InMessageType);

		AddToast();
	}
}

void UToastMessageBoxUI::SynchronizeProperties()
{
	Super::SynchronizeProperties();

#if WITH_EDITOR
	if (true == IsDesignTime())
	{	
		switch (ToastStackType)
		{
		case EToastStackType::Up:
		{
			if (nullptr != ToastBox) ToastBox->SetRenderTransformAngle(0.f);
		}
		break;

		case EToastStackType::Down:
		{
			if (nullptr != ToastBox) ToastBox->SetRenderTransformAngle(180.f);
		}
		break;

		default:
			break;
		}

		if (nullptr != ToastBox)
		{
			int PreviewNum = PreviewEntryCount;

			ToastBox->OnPreviewEntryCreatedFunc = [Type = ToastStackType](UUserWidget* InWidget)
				{
					if (UToastMessageTextUI* MessageUI = Cast<UToastMessageTextUI>(InWidget))
					{
						MessageUI->SetDirection(EToastStackType::Down == Type);
					}
				};

			ToastBox->Reset();
			for (int i = 0; i < PreviewEntryCount; ++i)
			{
				UToastMessageTextUI* MessageUI = ToastBox->CreateEntry<UToastMessageTextUI>();
				if (nullptr != MessageUI)
				{
					MessageUI->SetDirection(EToastStackType::Down == ToastStackType);
				}
			}
		}
	}
#endif

}

void UToastMessageBoxUI::NativeConstruct()
{
	Super::NativeConstruct();

	switch (ToastStackType)
	{
	case EToastStackType::Up:
	{
		if (nullptr != ToastBox) ToastBox->SetRenderTransformAngle(0.f);
	}
		break;

	case EToastStackType::Down:
	{
		if (nullptr != ToastBox) ToastBox->SetRenderTransformAngle(180.f);
	}
		break;
	default:
		break;
	}

}


