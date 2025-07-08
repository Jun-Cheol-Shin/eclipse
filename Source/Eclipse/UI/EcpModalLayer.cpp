// Fill out your copyright notice in the Description page of Project Settings.


#include "EcpModalLayer.h"
#include "Components/NamedSlot.h"
#include "Components/Image.h"

#include "../Subsystems/EcpUIManagerSubsystem.h"

void UEcpModalLayer::OnClick()
{
	if (nullptr != UIVariable.WidgetQueue)
	{

	}

	else if (nullptr != UIVariable.WidgetStack)
	{

	}
}

void UEcpModalLayer::OnConstruct()
{
	if (ensure(BackGroundImage))
	{
		BackGroundImage->SetVisibility(ESlateVisibility::Visible);
		DefaultColor = BackGroundImage->GetColorAndOpacity();
	}


	SetVisibility(ESlateVisibility::Collapsed);
}


void UEcpModalLayer::SetBlackScreen(bool bIsVisible)
{
	if (ensure(BackGroundImage))
	{
		BackGroundImage->SetColorAndOpacity(true == bIsVisible ? DefaultColor : FLinearColor::Transparent);
	}
}
