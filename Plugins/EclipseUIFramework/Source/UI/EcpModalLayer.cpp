// Fill out your copyright notice in the Description page of Project `tings.


#include "EcpModalLayer.h"
#include "Components/Border.h"

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
}

void UEcpModalLayer::OnConstruct()
{
	if (ensure(BackGroundImage))
	{
		BackGroundImage->SetVisibility(ESlateVisibility::Visible);
		DefaultColor = BackGroundImage->GetBrushColor();
	}
}

void UEcpModalLayer::SetBlackScreen(bool bIsVisible)
{
	if (ensure(BackGroundImage))
	{
		BackGroundImage->SetBrushColor(true == bIsVisible ? DefaultColor : FLinearColor::Transparent);
	}
}
