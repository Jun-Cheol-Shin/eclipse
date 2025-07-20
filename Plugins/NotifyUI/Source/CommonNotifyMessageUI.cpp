// Fill out your copyright notice in the Description page of Project Settings.


#include "CommonNotifyMessageUI.h"

#include "CommonRichTextBlock.h"
#include "CommonLazyImage.h"


void UCommonNotifyMessageUI::SetMessageStyle(const FNotifyStyleData& InStyle)
{
	if (InStyle.TextColor.IsValidIndex(0))
	{
		if (nullptr != UIVariable.TopMessage) UIVariable.TopMessage->SetDefaultColorAndOpacity(InStyle.TextColor[0]);
	}
	if (InStyle.TextColor.IsValidIndex(1))
	{
		if (nullptr != UIVariable.MiddleMessage) UIVariable.MiddleMessage->SetDefaultColorAndOpacity(InStyle.TextColor[1]);
	}
	if (InStyle.TextColor.IsValidIndex(2))
	{
		if (nullptr != UIVariable.BottomMessage) UIVariable.BottomMessage->SetDefaultColorAndOpacity(InStyle.TextColor[2]);
	}

	if (InStyle.FontInfo.IsValidIndex(0))
	{
		if (nullptr != UIVariable.TopMessage) UIVariable.TopMessage->SetDefaultFont(InStyle.FontInfo[0]);
	}

	if (InStyle.FontInfo.IsValidIndex(1))
	{
		if (nullptr != UIVariable.MiddleMessage) UIVariable.MiddleMessage->SetDefaultFont(InStyle.FontInfo[1]);
	}

	if (InStyle.FontInfo.IsValidIndex(2))
	{
		if (nullptr != UIVariable.BottomMessage) UIVariable.BottomMessage->SetDefaultFont(InStyle.FontInfo[2]);
	}

}

void UCommonNotifyMessageUI::SetMessageText(const FText& InText, const FText& InText_2, const FText& InText_3)
{
	SetMessageText(ETextDirection::Top, InText);

	SetMessageText(ETextDirection::Middle, InText_2);

	SetMessageText(ETextDirection::Bottom, InText_3);
}

void UCommonNotifyMessageUI::SetMessageText(ETextDirection InType, const FText& InText)
{
	UCommonRichTextBlock* SettingText = nullptr;

	switch (InType)
	{
	case ETextDirection::Top:
	{
		SettingText = UIVariable.TopMessage;
	}
	break;

	case ETextDirection::Middle:
	{
		SettingText = UIVariable.MiddleMessage;
	}
	break;

	case ETextDirection::Bottom:
	{
		SettingText = UIVariable.BottomMessage;
	}
		break;

	default:
	case ETextDirection::None:
		break;
	}



	if (nullptr != SettingText)
	{
		/*if (InText.IsEmpty())
		{
			SettingText->SetVisibility(ESlateVisibility::Collapsed);
		}

		else
		{
			SettingText->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			SettingText->SetText(InText);
		}*/

		SettingText->SetText(InText);
	}
}

void UCommonNotifyMessageUI::SetLazyIcon(const TSoftObjectPtr<UObject>& InSoftPtr)
{
	if (nullptr != UIVariable.Icon)
	{
		UIVariable.Icon->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		UIVariable.Icon->SetBrushFromLazyDisplayAsset(InSoftPtr);
	}
}

void UCommonNotifyMessageUI::SetDisbleIcon()
{
	if (nullptr != UIVariable.Icon)
	{
		UIVariable.Icon->SetVisibility(ESlateVisibility::Collapsed);
	}

}
