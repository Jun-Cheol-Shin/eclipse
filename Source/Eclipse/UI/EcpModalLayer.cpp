// Fill out your copyright notice in the Description page of Project Settings.


#include "EcpModalLayer.h"
#include "Components/NamedSlot.h"
#include "Components/Image.h"

#include "CommonActivatableWidget.h"

#include "Widgets/CommonActivatableWidgetContainer.h"
#include "../Subsystems/EcpUIManagerSubsystem.h"

void UEcpModalLayer::OnClick()
{
	if (false == ensure(UIVariable.WidgetStack)) return;

	if (UIVariable.WidgetStack->GetNumWidgets() > 0)
	{
		UCommonActivatableWidget* const& TopWidget = UIVariable.WidgetStack->GetWidgetList().Top();
		
		if (nullptr != TopWidget && nullptr != &TopWidget)
		{
			UIVariable.WidgetStack->RemoveWidget(*TopWidget);
		}
	}


	if (UIVariable.WidgetStack->GetNumWidgets() <= 0)
	{
		SetVisibility(ESlateVisibility::Collapsed);
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
