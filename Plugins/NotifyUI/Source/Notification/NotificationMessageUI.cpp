// Fill out your copyright notice in the Description page of Project Settings.


#include "NotificationMessageUI.h"

#include "CommonLazyImage.h"
#include "Components/TextBlock.h"

#include "Animation/WidgetAnimation.h"

void UNotificationMessageUI::RegistMessage(const FNotificationParams& InParam)
{
	if (ParamQueue.IsEmpty())
	{
		ParamQueue.Enqueue(InParam);
		CheckIconLoading(InParam);
	}

	else
	{
		ParamQueue.Enqueue(InParam);
	}


	/*if (false == TickDelegateHandle.IsValid())
	{
		FTSTicker::RemoveTicker(TickDelegateHandle);
		FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &UNotificationMessageUI::OnTicking), TickDelay);
	}*/
}

void UNotificationMessageUI::RegistCountDownMessage(const FNotificationParams& InParam, ECountDownTextType InCountDownTextType, const FTextFormat& InFormat, double InCountDown)
{
	CountDownTextType = InCountDownTextType;
	CountDown = InCountDown;
	CountDownTextFormat = InFormat;

	// todo
	switch (CountDownTextType)
	{
	case ECountDownTextType::Top:
	{
		if (TopMessage)
		{
			TopMessage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			//TopMessage->SetText(FText::Format());
		}
	}
		break;
	case ECountDownTextType::Middle:
	{
		if (MiddleMessage) MiddleMessage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
		break;
	case ECountDownTextType::Bottom:
	{
		if (BottomMessage) BottomMessage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
		break;
	default:
		break;
	}

	RegistMessage(InParam);
}

void UNotificationMessageUI::CheckIconLoading(const FNotificationParams& InParam)
{
	TSoftObjectPtr<UObject> IconImage = nullptr;
	if (false == GetIconType(InParam.MessageType, &IconImage))
	{
		if (ensure(Icon)) Icon->SetVisibility(ESlateVisibility::Collapsed);
		SetNextMessage(InParam);
	}

	else
	{
		// todo
		if (ensure(Icon))
		{
			Icon->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			Icon->SetBrushFromLazyDisplayAsset(IconImage);

			// goto OnChangedLoadingState
		}
	}
}

void UNotificationMessageUI::SetNextMessage(const FNotificationParams& InParam)
{
	SetMessageText(InParam.TopMessage, TopMessage);
	SetMessageText(InParam.MiddleMessage, MiddleMessage);
	SetMessageText(InParam.BottomMessage, BottomMessage);

	CurrentMessageDuration = InParam.RegistTime + Duration;
	SetVisibility(true == bTouchable ? ESlateVisibility::Visible : ESlateVisibility::HitTestInvisible);

	if (bAnimating)
	{
		CurrentMessageDuration += FadeIn->GetEndTime();
		PlayAnimationForward(FadeIn);
	}

	else
	{
		FTSTicker::RemoveTicker(TickDelegateHandle);
		FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &UNotificationMessageUI::OnTicking), TickDelay);
	}
}

void UNotificationMessageUI::SetMessageText(const FText& InParamText, UTextBlock* InTextBlock)
{
	if (nullptr != InTextBlock)
	{
		if (InParamText.IsEmpty())
		{
			InTextBlock->SetVisibility(ESlateVisibility::Collapsed);
		}

		else
		{
			InTextBlock->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			InTextBlock->SetText(InParamText);
		}
	}
}

void UNotificationMessageUI::SetCountDownText(int64 InCountDownSec)
{

}

bool UNotificationMessageUI::GetIconType(const FGameplayTag& InMessageType, OUT TSoftObjectPtr<UObject>* OutImageSource)
{
	//*OutImageSource = 

	return false;
}

void UNotificationMessageUI::OnEndedFadeOutAnimation()
{
	SetVisibility(ESlateVisibility::Collapsed);
	OnEndedNotify.ExecuteIfBound();


	if (false == ParamQueue.IsEmpty())
	{
		FNotificationParams NewParam;
		ParamQueue.Dequeue(OUT NewParam);
		SetNextMessage(NewParam);
	}
}

void UNotificationMessageUI::OnEnededFadeInAnimation()
{
	FTSTicker::RemoveTicker(TickDelegateHandle);
	FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &UNotificationMessageUI::OnTicking), TickDelay);
}

bool UNotificationMessageUI::OnTicking(float InDeltaTime)
{
	if (CurrentMessageDuration <= FSlateApplication::Get().GetCurrentTime())
	{
		if (true == bAnimating)
		{
			PlayAnimationForward(FadeOut);
			return false;
		}

		else
		{
			SetVisibility(ESlateVisibility::Collapsed);
			OnEndedNotify.ExecuteIfBound();

			if (false == ParamQueue.IsEmpty())
			{
				FNotificationParams NewParam;
				ParamQueue.Dequeue(OUT NewParam);
				SetNextMessage(NewParam);
			}

			return false;
		}
	}

	else
	{
		if (false == FMath::IsNegativeOrNegativeZero(CountDown))
		{

		}
	}


	return true;
}

void UNotificationMessageUI::OnChangedLoadingState(bool bIsLoading)
{
	if (true == bIsLoading) return;





}

void UNotificationMessageUI::NativeConstruct()
{
	Super::NativeConstruct();

	if (true == bAnimating)
	{
		if (ensure(FadeIn))
		{
			FadeInAnimationEvent.Clear();
			FadeInAnimationEvent.BindDynamic(this, &UNotificationMessageUI::OnEnededFadeInAnimation);
			BindToAnimationFinished(FadeOut, FadeInAnimationEvent);
		}

		if (ensure(FadeOut))
		{
			FadeOutAnimationEvent.Clear();
			FadeOutAnimationEvent.BindDynamic(this, &UNotificationMessageUI::OnEndedFadeOutAnimation);
			BindToAnimationFinished(FadeOut, FadeOutAnimationEvent);
		}

	}


	if (nullptr != Icon)
	{
		Icon->OnLoadingStateChanged().AddUObject(this, &UNotificationMessageUI::OnChangedLoadingState);
	}

	FTSTicker::RemoveTicker(TickDelegateHandle);

}

void UNotificationMessageUI::NativeDestruct()
{

	FTSTicker::RemoveTicker(TickDelegateHandle);

	if (nullptr != Icon)
	{
		Icon->OnLoadingStateChanged().RemoveAll(this);
	}

	if (nullptr != FadeOut && FadeOutAnimationEvent.IsBound())
	{
		UnbindAllFromAnimationFinished(FadeOut);
		FadeOutAnimationEvent.Clear();
	}

	if (nullptr != FadeIn && FadeInAnimationEvent.IsBound())
	{
		UnbindAllFromAnimationFinished(FadeIn);
		FadeInAnimationEvent.Clear();
	}

	Super::NativeDestruct();
}

