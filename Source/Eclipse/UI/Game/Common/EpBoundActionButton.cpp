// Fill out your copyright notice in the Description page of Project Settings.


#include "EpBoundActionButton.h"

#include "CommonLazyImage.h"
#include "CommonTextBlock.h"
#include "CommonActionWidget.h"

#include "Components/VerticalBoxSlot.h"
#include "Components/SizeBox.h"

void UEpBoundActionButton::NativeOnChangedTabState(EEpButtonState InState)
{
	SetButtonState(InState);
}

void UEpBoundActionButton::NativeOnSetTabInfo(const FEpTabParameter& TabDescriptor)
{
	if (nullptr != IconWrapperBox)
	{
		if (false == TabDescriptor.bShowTabIcon)
		{
			IconWrapperBox->SetVisibility(ESlateVisibility::Collapsed);
		}

		else
		{
			if (nullptr != TabButtonIcon)
			{
				if (TabDescriptor.bShowTabIcon)
				{
					IconWrapperBox->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
					TabButtonIcon->SetBrushFromLazyTexture(TabDescriptor.TabIcon);
				}
			}
		}
	}

	if (nullptr != ActionWrapperBox)
	{
		ActionWrapperBox->SetWidthOverride(TabDescriptor.ActionIconSize.X);
		ActionWrapperBox->SetHeightOverride(TabDescriptor.ActionIconSize.Y);
	}

	if (nullptr != ActionWidget)
	{
		if (UVerticalBoxSlot* OverlaySlot = Cast<UVerticalBoxSlot>(ActionWidget->Slot))
		{
			OverlaySlot->SetPadding(TabDescriptor.ActionIconOffset);
		}

		ActionWidget->SetEnhancedInputAction(TabDescriptor.InputAction);
	}

	if (nullptr != Text_ActionName)
	{
		FSlateFontInfo NewFontInfo = Text_ActionName->GetFont();
		NewFontInfo.Size = TabDescriptor.FontSize;
		Text_ActionName->SetFont(NewFontInfo);
		Text_ActionName->SetText(TabDescriptor.TabText);
	}
}

void UEpBoundActionButton::SetButtonState(EEpButtonState InState)
{
	if (ButtonState == InState) return;

	ButtonState = InState;

	switch (ButtonState)
	{
	case EEpButtonState::Disable:
	{
		SetIsEnabled(false);
		SetIsSelectable(false);
		SetIsSelected(false);
	}
	break;
	case EEpButtonState::Enable:
	{
		SetIsEnabled(true);
		if (IsDesignTime())
		{
			SetIsSelectable(false);
		}
		else
		{
			SetIsSelectable(true);
		}
		SetIsSelected(false);
	}
	break;
	case EEpButtonState::Selected:
	{
		SetIsEnabled(true);
		SetIsSelectable(true);
		SetIsSelected(true);
	}
	break;
	default:
		break;
	}
}

void UEpBoundActionButton::NativeOnClicked()
{
	Super::NativeOnClicked();

	if (ButtonState == EEpButtonState::Disable)
	{
		// Show System Msg..

	}
}

void UEpBoundActionButton::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	SetButtonState(ButtonState);
}
