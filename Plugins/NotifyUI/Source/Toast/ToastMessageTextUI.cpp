// Fill out your copyright notice in the Description page of Project Settings.


#include "ToastMessageTextUI.h"
#include "ToastMessageBoxUI.h"
#include "Components/SizeBox.h"

void UToastMessageTextUI::PlayToastAnimation(EAnimationType InType)
{
	switch (InType)
	{
	case EAnimationType::Up:
		if (FMath::IsNearlyZero(GetRenderTransformAngle()))
		{
			if (nullptr != Up) PlayAnimationForward(Up);
		}

		else
		{
			if (nullptr != Down) PlayAnimationForward(Down);
		}
		break;

	case EAnimationType::Down:
		if (FMath::IsNearlyZero(GetRenderTransformAngle()))
		{
			if (nullptr != Down) PlayAnimationForward(Down);
		}

		else
		{
			if (nullptr != Up) PlayAnimationForward(Up);
		}

		break;

	case EAnimationType::Left:
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

	case EAnimationType::Right:
	{
		if (FMath::IsNearlyZero(GetRenderTransformAngle()))
		{
			if (nullptr != Right) PlayAnimationForward(Right);
		}

		else
		{
			if (nullptr != Left) PlayAnimationForward(Left);
		}
	}
		break;

	default:
		break;
	}
}

void UToastMessageTextUI::SetStackDirection(EToastStackType InStackType)
{
	if (nullptr != SizeBox)
	{
		SizeBox->SetRenderTransformAngle(EToastStackType::Down == InStackType ? 180.f : 0.f);
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

void UToastMessageTextUI::OnCompleteFadeInAnimation()
{
	SetVisibility(ESlateVisibility::Collapsed);

	if (OnCompleteAnimEndDelegate.IsBound())
	{
		OnCompleteAnimEndDelegate.ExecuteIfBound(this);
	}
}
