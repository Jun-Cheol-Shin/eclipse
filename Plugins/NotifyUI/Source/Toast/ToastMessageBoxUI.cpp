// Fill out your copyright notice in the Description page of Project Settings.


#include "ToastMessageBoxUI.h"
#include "ToastMessageTextUI.h"
#include "Components/DynamicEntryBox.h"



void UToastMessageBoxUI::RegistToast(FGameplayTag InTag, const FText& InText)
{
	if (nullptr == ToastBox) return;

	if (ToastBoxMaximumCount > ToastBox->GetAllEntries().Num())
	{
		AddToast(InTag, InText);
	}

	else
	{
		WaitingQueue.Enqueue({InTag, InText});

		RemoveToast();
	}

}

void UToastMessageBoxUI::AddToast(const FGameplayTag& InTag, const FText& InText)
{
	UToastMessageTextUI* MessageUI = ToastBox->CreateEntry<UToastMessageTextUI>();
	if (nullptr != MessageUI)
	{
		MessageQueue.Emplace(InTag, FPlatformTime::Seconds());

		MessageUI->OnCompleteAnimEndDelegate.BindUObject(this, &UToastMessageBoxUI::OnEndedFadeOutAnimation);
		MessageUI->Start(InText, EToastStackType::Down == ToastStackType);
	}
}

void UToastMessageBoxUI::RemoveToast()
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



}


void UToastMessageBoxUI::OnEndedFadeOutAnimation(UToastMessageTextUI* InMessageText)
{
	if (nullptr == ToastBox) return;

	ToastBox->RemoveEntry(InMessageText);
	TPair<FGameplayTag, double> EndMessage = MessageQueue.Pop();
	OnFinishMessageSignature.Broadcast(EndMessage.Key);

	if (TPair<FGameplayTag, FText>* NextParam = WaitingQueue.Peek())
	{
		WaitingQueue.Pop();
		AddToast(NextParam->Key, NextParam->Value);
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


