// Fill out your copyright notice in the Description page of Project Settings.


#include "ToastMessageBoxUI.h"
#include "ToastMessageTextUI.h"
#include "Components/DynamicEntryBox.h"


void UToastMessageBoxUI::SynchronizeProperties()
{
	Super::SynchronizeProperties();

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

	if (true == IsDesignTime())
	{	
		if (nullptr != ToastBox)
		{
			int PreviewNum = PreviewEntryCount;

			ToastBox->OnPreviewEntryCreatedFunc = [Type = ToastStackType](UUserWidget* InWidget)
				{
					if (UToastMessageTextUI* MessageUI = Cast<UToastMessageTextUI>(InWidget))
					{
						MessageUI->SetStackDirection(Type);
					}
				};

			ToastBox->Reset();
			for (int i = 0; i < PreviewEntryCount; ++i)
			{
				UToastMessageTextUI* MessageUI = ToastBox->CreateEntry<UToastMessageTextUI>();
				if (nullptr != MessageUI)
				{
					MessageUI->SetStackDirection(ToastStackType);
				}
			}
		}
	}
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

void UToastMessageBoxUI::RegistToast(FGameplayTag InTag, const FText& InText)
{
	if (nullptr == ToastBox) return;

	if (ToastBoxMaximumCount > ToastBox->GetAllEntries().Num())
	{
		UToastMessageTextUI* MessageUI = ToastBox->CreateEntry<UToastMessageTextUI>();
		if (nullptr != MessageUI)
		{
			MessageUI->SetStackDirection(ToastStackType);
			MessageUI->OnCompleteAnimEndDelegate.BindUObject(this, &UToastMessageBoxUI::OnEndedFadeOutAnimation);
			MessageUI->PlayToastAnimation(ToastAnimationType);
			MessageUI->SetMessageText(InText);
		}
	}

	else
	{
		const TArray<UToastMessageTextUI*>& EntryList = ToastBox->GetTypedEntries<UToastMessageTextUI>();

		if (EntryList.IsValidIndex(0))
		{
			UToastMessageTextUI* MessageUI = EntryList[0];
			MessageUI->OnCompleteAnimEndDelegate.Unbind();
			ToastBox->RemoveEntry(EntryList[0]);
		}

		UToastMessageTextUI* MessageUI = ToastBox->CreateEntry<UToastMessageTextUI>();
		if (nullptr != MessageUI)
		{
			MessageUI->SetStackDirection(ToastStackType);
			MessageUI->OnCompleteAnimEndDelegate.BindUObject(this, &UToastMessageBoxUI::OnEndedFadeOutAnimation);
			MessageUI->PlayToastAnimation(ToastAnimationType);
			MessageUI->SetMessageText(InText);
		}
	}

}

void UToastMessageBoxUI::OnEndedFadeOutAnimation(UToastMessageTextUI* InMessageText)
{
	if (nullptr == ToastBox) return;

	ToastBox->RemoveEntry(InMessageText);
}
