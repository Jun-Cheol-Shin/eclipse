// Fill out your copyright notice in the Description page of Project Settings.


#include "NotificationMessageUI.h"

#include "CommonLazyImage.h"
#include "Components/TextBlock.h"

#include "Animation/WidgetAnimation.h"

void UNotificationMessageUI::RegistMessage(const FGameplayTag& InType, const FText& InMessage)
{
	FNotificationParams NewParam;
	NewParam.MessageType = InType;
	NewParam.RegistTime = FSlateApplication::Get().GetCurrentTime();
	NewParam.TopMessage = InMessage;

	RegistMessage(NewParam);
}

void UNotificationMessageUI::RegistTwoMessage(const FGameplayTag& InType, const FText& InMessage, const FText& InMessageTwo)
{
	FNotificationParams NewParam;
	NewParam.MessageType = InType;
	NewParam.RegistTime = FSlateApplication::Get().GetCurrentTime();
	NewParam.TopMessage = InMessage;
	NewParam.MiddleMessage = InMessageTwo;

	RegistMessage(NewParam);
}

void UNotificationMessageUI::RegistThreeMessage(const FGameplayTag& InType, const FText& InMessage, const FText& InMessageTwo, const FText& InMessageThree)
{
	FNotificationParams NewParam;
	NewParam.MessageType = InType;
	NewParam.RegistTime = FSlateApplication::Get().GetCurrentTime();
	NewParam.TopMessage = InMessage;
	NewParam.MiddleMessage = InMessageTwo;
	NewParam.BottomMessage = InMessageThree;

	RegistMessage(NewParam);
}

void UNotificationMessageUI::UnregistMessage(const FGameplayTag& InTag)
{
	UnregistTagSet.Add(InTag);
}

void UNotificationMessageUI::RegistCountDownMessage(const FGameplayTag& InType, const FString& InFormat, double InCountDown, const FText& InMessage, const FText& InMessageTwo)
{
	FNotificationParams NewParam(InType, FSlateApplication::Get().GetCurrentTime(), FTextFormat::FromString(InFormat), InCountDown, InMessage, InMessageTwo);
	RegistMessage(NewParam);
}

void UNotificationMessageUI::RegistMessage(const FNotificationParams& InParam)
{
	if (false == IsVisible())
	{
		ParamQueue.Enqueue(InParam);
		NextMessage();
	}

	else
	{
		ParamQueue.Enqueue(InParam);
	}
}


void UNotificationMessageUI::NextMessage()
{
	if (ParamQueue.IsEmpty())
	{
		SetVisibility(ESlateVisibility::Collapsed);
		UnregistTagSet.Reset();

		return;
	}

	FNotificationParams StartParam;
	ParamQueue.Dequeue(OUT StartParam);

	if (UnregistTagSet.Contains(StartParam.MessageType))
	{
		UnregistTagSet.Remove(StartParam.MessageType);
		NextMessage();
	}
	
	else
	{

		TSoftObjectPtr<UObject> IconImage = nullptr;
		if (false == GetIconType(StartParam.MessageType, &IconImage))
		{
			if (ensure(Icon)) Icon->SetVisibility(ESlateVisibility::Collapsed);
			SetMessage(StartParam);
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

}

void UNotificationMessageUI::SetMessage(const FNotificationParams& InParam)
{
	CurrentParam = InParam;
	SetMessageText(InParam.TopMessage, TopMessage);
	SetMessageText(InParam.MiddleMessage, MiddleMessage);
	SetMessageText(InParam.BottomMessage, BottomMessage);

	CurrentDuration = FSlateApplication::Get().GetCurrentTime() + Duration;
	double CurrentCountDown = InParam.CountDown - (FSlateApplication::Get().GetCurrentTime() - InParam.RegistTime);
	CurrentShowingCountDown = FMath::CeilToInt64(CurrentCountDown);
	SetCountdownMessage(CurrentShowingCountDown);

	SetVisibility(true == bTouchable ? ESlateVisibility::Visible : ESlateVisibility::HitTestInvisible);

	if (bAnimating)
	{
		CurrentDuration += FadeIn->GetEndTime();
		PlayAnimationForward(FadeIn);
	}

	else
	{
		OnEnededFadeInAnimation();
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


bool UNotificationMessageUI::GetIconType(const FGameplayTag& InMessageType, OUT TSoftObjectPtr<UObject>* OutImageSource)
{
	//*OutImageSource = 

	return false;
}

void UNotificationMessageUI::SetCountdownMessage(int64 InShowCountDownSec)
{
	switch (CurrentParam.CountTextType)
	{
	case ECountDownTextDirection::Bottom:
	{
		SetMessageText(FText::Format(CurrentParam.Format, InShowCountDownSec), BottomMessage);
	}
		break;

	case ECountDownTextDirection::Top:
	{
		SetMessageText(FText::Format(CurrentParam.Format, InShowCountDownSec), TopMessage);
	}
		break;

	case ECountDownTextDirection::Middle:
	{
		SetMessageText(FText::Format(CurrentParam.Format, InShowCountDownSec), MiddleMessage);
	}
		break;

	default:
	case ECountDownTextDirection::None:
		break;
	}
}

void UNotificationMessageUI::OnEndedFadeOutAnimation()
{
	OnEndedNotify.ExecuteIfBound();
	NextMessage();
}

void UNotificationMessageUI::OnEnededFadeInAnimation()
{
	FTSTicker::RemoveTicker(TickDelegateHandle);
	TickDelegateHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &UNotificationMessageUI::OnTicking), TickDelay);
}

bool UNotificationMessageUI::OnTicking(float InDeltaTime)
{
	if (CurrentDuration <= FSlateApplication::Get().GetCurrentTime())
	{
		if (true == bAnimating)
		{
			PlayAnimationForward(FadeOut);
			return false;
		}

		else
		{
			OnEndedFadeOutAnimation();
			return false;
		}
	}

	else
	{
		double NowCalculatedCountDown = CurrentParam.CountDown - (FSlateApplication::Get().GetCurrentTime() - CurrentParam.RegistTime);
		int64 NowCalculatedCountDownInt = FMath::CeilToInt64(NowCalculatedCountDown);

		if (NowCalculatedCountDownInt <= 0)
		{
			CurrentShowingCountDown = 0;
			SetCountdownMessage(0);
		}

		else if (NowCalculatedCountDownInt < CurrentShowingCountDown)
		{
			CurrentShowingCountDown = NowCalculatedCountDownInt;
			SetCountdownMessage(CurrentShowingCountDown);
		}
	}


	return true;
}

void UNotificationMessageUI::OnChangedLoadingState(bool bIsLoading)
{
	if (true == bIsLoading) return;

	// todo
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
			BindToAnimationFinished(FadeIn, FadeInAnimationEvent);
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

	SetVisibility(ESlateVisibility::Collapsed);


	SetConsumePointerInput(bTouchable);
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

FReply UNotificationMessageUI::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InPointEvent)
{
	//OnTouchDelegate

	return FReply::Handled();
}

