// Fill out your copyright notice in the Description page of Project Settings.


#include "EpBoundActionButton.h"

#include "CommonLazyImage.h"
#include "CommonTextBlock.h"

void UEpBoundActionButton::NativeOnChangedTabState(EEpButtonState InState)
{
	SetButtonState(InState);
}

void UEpBoundActionButton::NativeOnSetTabInfo(const FEpTabParameter& TabDescriptor)
{
	if (nullptr != TabButtonIcon)
	{
		TabButtonIcon->SetBrushFromLazyTexture(TabDescriptor.TabIcon);
	}

	if (nullptr != Text_ActionName)
	{
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
	}
	break;
	case EEpButtonState::Enable:
	{
		SetIsEnabled(true);
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
