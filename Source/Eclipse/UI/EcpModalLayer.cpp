// Fill out your copyright notice in the Description page of Project Settings.


#include "EcpModalLayer.h"
#include "Components/NamedSlot.h"
#include "Components/Image.h"

#include "../Subsystems/EcpUIManagerSubsystem.h"

void UEcpModalLayer::OnClick()
{
	if (UEcpUIManagerSubsystem* UISubsystem = GetGameInstanceChecked().GetSubsystem<UEcpUIManagerSubsystem>())
	{
		// todo : Hide


	}
}

void UEcpModalLayer::OnConstruct()
{
	if (ensure(BackGroundImage))
	{
		BackGroundImage->SetVisibility(ESlateVisibility::Visible);
		DefaultColor = BackGroundImage->GetColorAndOpacity();
	}

	if (ensure(PopupWidget))
	{
		PopupWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

void UEcpModalLayer::NativeOnActivated()
{
	//RegisterScrollRecipient();
}

void UEcpModalLayer::NativeOnDeactivated()
{
	//UnregisterScrollRecipient();
}

void UEcpModalLayer::SetBlackScreen(bool bIsVisible)
{
	if (ensure(BackGroundImage))
	{
		BackGroundImage->SetColorAndOpacity(true == bIsVisible ? DefaultColor : FLinearColor::Transparent);
	}
}
