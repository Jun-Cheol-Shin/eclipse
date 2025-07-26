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

void UToastMessageBoxUI::RegistToastInternal(FGameplayTag InTag, const FTextFormat& InFormat, FFormatOrderedArguments InArguments)
{
	if (nullptr == ToastBox) return;

	if (true == bMerge)
	{
		FToastMessageData NewData(InTag, InFormat, InArguments);
		if (false == NeedMergeData.Key.IsValid())
		{
			NeedMergeData = { NewData, FPlatformTime::Seconds() + ToastMergeInterval };
		}


		else if (true == NeedMergeData.Key.IsValid() && true == NeedMergeData.Key.IsPossibleMerge(NewData))
		{
			// merge...
			NeedMergeData.Key.Merge(NewData);
		}

		else if (true == NeedMergeData.Key.IsValid())
		{
			UE_LOG(LogTemp, Warning, TEXT(" Merge TEXT! "));

			// reset wait data...
			WaitQueue.Enqueue(NeedMergeData.Key);
			NeedMergeData = { NewData, FPlatformTime::Seconds() + ToastMergeInterval };
		}
	}

	else
	{
		RegistToastInternal(InTag, FText::Format(InFormat, InArguments));
	}
}

void UToastMessageBoxUI::RegistToastInternal(FGameplayTag InTag, const FText& InText)
{
	if (nullptr == ToastBox) return;

	WaitQueue.Enqueue(FToastMessageData(InTag, InText));

	if (ToastBoxMaximumCount <= ToastBox->GetAllEntries().Num())
	{
		RemoveToast(0);
	}

	else
	{
		AddToast();
	}

}

void UToastMessageBoxUI::AddToast()
{
	UE_LOG(LogTemp, Warning, TEXT(" Add TEXT! "));

	if (nullptr == WaitQueue.Peek()) return;

	FToastMessageData NewData;
	WaitQueue.Dequeue(OUT NewData);

	UToastMessageTextUI* MessageUI = ToastBox->CreateEntry<UToastMessageTextUI>();
	if (nullptr != MessageUI)
	{
		TimeQueue.Emplace(FPlatformTime::Seconds() + ToastDuration, false);

		MessageUI->OnCompleteAnimEndDelegate.BindUObject(this, &UToastMessageBoxUI::OnEndedFadeOutAnimation);
		MessageUI->Start(NewData, EToastStackType::Down == ToastStackType);
	}
}

/*
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
}*/

void UToastMessageBoxUI::RemoveToast(int Index)
{
	const TArray<UToastMessageTextUI*>& EntryList = ToastBox->GetTypedEntries<UToastMessageTextUI>();

	if (EntryList.IsValidIndex(Index))
	{
		UToastMessageTextUI* MessageUI = EntryList[Index];

		if (nullptr != MessageUI)
		{
			MessageUI->End();
		}
	}
}

void UToastMessageBoxUI::NativeTick(const FGeometry& InGeometry, float InDeltaTime)
{
	Super::NativeTick(InGeometry, InDeltaTime);

	if (true ==  NeedMergeData.Key.IsValid() && NeedMergeData.Value <= FPlatformTime::Seconds())
	{
		UE_LOG(LogTemp, Warning, TEXT(" Not Merge... "));

		WaitQueue.Enqueue(NeedMergeData.Key);
		NeedMergeData = { FToastMessageData(), 0.f};
		if (ToastBoxMaximumCount <= ToastBox->GetAllEntries().Num())
		{
			RemoveToast(0);
		}

		else
		{
			AddToast();
		}
	}

	// 같은 시간에 연속으로 들어왔다면 어떻게 해야 할까?
	for (int i = 0; i < TimeQueue.Num(); ++i)
	{
		if (TimeQueue[i].Key <= FPlatformTime::Seconds())
		{
			// is expired
			TimeQueue[i].Value = true;
			RemoveToast(i);
		}
	}

	TimeQueue.RemoveAll([](const TPair<double, bool>& InPair)
		{
			return true == InPair.Value;
		});

	/*
	* 뒤에서 순회하면 for문 내부에서 제거해도 문제가 안된다
	for (int32 Index = A.Num()-1; Index >= 0; --Index)
	{
		if (A[Index] == nullptr)
		{
			const bool bAllowShrinking = false;
			A.RemoveAt(Index, 1, bAllowShrinking);
		}
	}
	*/
}

void UToastMessageBoxUI::OnEndedFadeOutAnimation(UToastMessageTextUI* InMessageText, const FGameplayTag& InMessageType)
{
	if (nullptr == ToastBox) return;

	ToastBox->RemoveEntry(InMessageText);
	OnFinishMessageSignature.Broadcast(InMessageType);

	AddToast();
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


