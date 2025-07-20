// Fill out your copyright notice in the Description page of Project Settings.


#include "CommonNotifyMessageUI.h"

#include "Components/TextBlock.h"
#include "CommonLazyImage.h"


void UCommonNotifyMessageUI::SetMessageStyle(const FNotifyStyleData& InStyle)
{
	if (InStyle.TextColor.IsValidIndex(0))
	{
		TopMessage->SetColorAndOpacity(InStyle.TextColor[0]);
	}
	if (InStyle.TextColor.IsValidIndex(1))
	{
		MiddleMessage->SetColorAndOpacity(InStyle.TextColor[1]);
	}
	if (InStyle.TextColor.IsValidIndex(2))
	{
		BottomMessage->SetColorAndOpacity(InStyle.TextColor[2]);
	}

	if (InStyle.FontInfo.IsValidIndex(0))
	{
		TopMessage->SetFont(InStyle.FontInfo[0]);
	}

	if (InStyle.FontInfo.IsValidIndex(1))
	{
		MiddleMessage->SetFont(InStyle.FontInfo[1]);
	}

	if (InStyle.FontInfo.IsValidIndex(2))
	{
		BottomMessage->SetFont(InStyle.FontInfo[2]);
	}

}


void UCommonNotifyMessageUI::SetMessageText(ETextDirection InType, const FText& InText)
{
	UTextBlock* SettingText = nullptr;

	switch (InType)
	{
	case ETextDirection::Top:
	{
		SettingText = TopMessage;
	}
	break;

	case ETextDirection::Middle:
	{
		SettingText = MiddleMessage;
	}
	break;

	case ETextDirection::Bottom:
	{
		SettingText = BottomMessage;
	}
		break;

	default:
	case ETextDirection::None:
		break;
	}



	if (nullptr != SettingText)
	{
		if (InText.IsEmpty())
		{
			SettingText->SetVisibility(ESlateVisibility::Collapsed);
		}

		else
		{
			SettingText->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			SettingText->SetText(InText);
		}
	}
}

void UCommonNotifyMessageUI::SetLazyIcon(const TSoftObjectPtr<UObject>& InSoftPtr)
{
	Icon->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	Icon->SetBrushFromLazyDisplayAsset(InSoftPtr);
}

void UCommonNotifyMessageUI::SetDisbleIcon()
{
	Icon->SetVisibility(ESlateVisibility::Collapsed);
}
