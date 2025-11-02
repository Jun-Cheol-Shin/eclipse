// Fill out your copyright notice in the Description page of Project Settings.


#include "EpBoundActionButton.h"

void UEpBoundActionButton::NativeOnSetTabInfo(const FEpTabParameter& TabDescriptor)
{

}

void UEpBoundActionButton::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	switch (ButtonState)
	{
	case EEpButtonState::Disable:
	{
		SetIsEnabled(false);
		//SetIsSelected(false);
	}
		break;
	case EEpButtonState::Enable:
	{
		SetIsEnabled(true);
		//SetIsSelected(false);
	}
		break;
	case EEpButtonState::Selected:
	{
		SetIsEnabled(true);

		//SetStyle(ButtonStyle);
	}
		break;
	default:
		break;
	}
}
