// Fill out your copyright notice in the Description page of Project Settings.


#include "ToastMessageTextUI.h"
#include "ToastMessageBoxUI.h"
#include "Components/SizeBox.h"


void UToastMessageTextUI::Start(const FToastMessageData& InData, bool bReversed)
{
	MessageData = InData;

	SetMessageText(ETextDirection::Top, FText::Format(InData.Format, InData.Arguments));
	SetDirection(bReversed);
	PlayToastAnimation();
}

void UToastMessageTextUI::End()
{
	if (nullptr != FadeOut)
	{
		PlayAnimationForward(FadeOut);
	}
}

void UToastMessageTextUI::SetDirection(bool bReversed)
{
	if (nullptr != SizeBox)
	{
		SizeBox->SetRenderTransformAngle(true == bReversed ? 180.f : 0.f);
	}
}

void UToastMessageTextUI::NativeConstruct()
{
	Super::NativeConstruct();

	if (nullptr != FadeOut)
	{
		FWidgetAnimationDynamicEvent FadeoutEvent;
		FadeoutEvent.BindDynamic(this, &UToastMessageTextUI::OnCompleteFadeInAnimation);
		BindToAnimationFinished(FadeOut, FadeoutEvent);
	}
}

void UToastMessageTextUI::NativeDestruct()
{
	Super::NativeDestruct();

	if (nullptr != FadeOut)
	{
		UnbindAllFromAnimationFinished(FadeOut);
	}
}

void UToastMessageTextUI::RemoveFromParent()
{
	OnCompleteAnimEndDelegate.Unbind();
}

void UToastMessageTextUI::OnCompleteFadeInAnimation()
{
	SetVisibility(ESlateVisibility::Collapsed);

	OnCompleteAnimEndDelegate.ExecuteIfBound(this, MessageData.MessageType);
}

void UToastMessageTextUI::PlayToastAnimation()
{
	switch (AnimationType)
	{
	case EAnimationType::FadeIn_Veritcal:
		if (FMath::IsNearlyZero(GetRenderTransformAngle()))
		{
			if (nullptr != Up) PlayAnimationForward(Up);
		}

		else
		{
			if (nullptr != Down) PlayAnimationForward(Down);
		}
		break;

	case EAnimationType::FadeIn_Horizontal:
	{
		if (FMath::IsNearlyZero(GetRenderTransformAngle()))
		{
			if (nullptr != Left) PlayAnimationForward(Left);
		}

		else
		{
			if (nullptr != Right) PlayAnimationForward(Right);
		}
	}
	break;

	default:
		break;
	}
}