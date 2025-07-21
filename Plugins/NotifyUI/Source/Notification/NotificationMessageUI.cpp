// Fill out your copyright notice in the Description page of Project Settings.


#include "NotificationMessageUI.h"
#include "Animation/WidgetAnimation.h"

DEFINE_LOG_CATEGORY(NotityUILog);

void UNotificationMessageUI::RegistOneMessage(FGameplayTag InType, const FText& InMessage)
{
	FNotificationParams NewParam(InType, FPlatformTime::Seconds(), InMessage);
	RegistMessageInternal(NewParam);
}

void UNotificationMessageUI::RegistTwoMessage(FGameplayTag InType, const FText& InMessage, const FText& InMessage_2)
{
	FNotificationParams NewParam(InType, FPlatformTime::Seconds(), InMessage, InMessage_2);
	RegistMessageInternal(NewParam);
}

void UNotificationMessageUI::RegistThreeMessage(FGameplayTag InType, const FText& InMessage, const FText& InMessage_2, const FText& InMessage_3)
{
	FNotificationParams NewParam(InType, FPlatformTime::Seconds(), InMessage, InMessage_2, InMessage_3);
	RegistMessageInternal(NewParam);
}

void UNotificationMessageUI::RegistTopCountMessage(FGameplayTag InType, const FString& InFormat, double InCountDown)
{
	FNotificationParams NewParam(InType, FPlatformTime::Seconds(), FTextFormat::FromString(InFormat), InCountDown);
	RegistMessageInternal(NewParam);
}

void UNotificationMessageUI::RegistMiddleCountMessage(FGameplayTag InType, const FText& InTopMessage, const FString& InFormat, double InCountDown)
{
	FNotificationParams NewParam(InType, FPlatformTime::Seconds(), InTopMessage, FTextFormat::FromString(InFormat), InCountDown);
	RegistMessageInternal(NewParam);
}

void UNotificationMessageUI::RegistBottomCountMessage(FGameplayTag InType, const FText& InTopMessage, const FText& InMiddleMessage, const FString& InFormat, double InCountDown)
{
	FNotificationParams NewParam(InType, FPlatformTime::Seconds(), InTopMessage, InMiddleMessage, FTextFormat::FromString(InFormat), InCountDown);
	RegistMessageInternal(NewParam);
}

void UNotificationMessageUI::UnregistMessage(FGameplayTag InTag)
{
	UnregistTagSet.Add(InTag);
}


void UNotificationMessageUI::RegistMessageInternal(const FNotificationParams& InParam)
{
	if (CurrentQueueSize >= MaximumQueueSize)
	{
		UE_LOG(NotityUILog, Warning, TEXT("Message Queue is full."))
		return;
	}

	++CurrentQueueSize;
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

	--CurrentQueueSize;

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
			if (nullptr != Message) Message->SetDisbleIcon();
			//if (ensure(Icon)) Icon->SetVisibility(ESlateVisibility::Collapsed);
		}

		else
		{
			if (nullptr != Message) Message->SetLazyIcon(IconImage);
		}

		SetMessage(StartParam);
	}

}

void UNotificationMessageUI::SetMessage(const FNotificationParams& InParam)
{
	CurrentParam = InParam;

	if (nullptr != Message)
	{
		CurrentDuration = FPlatformTime::Seconds() + Duration;
		double CurrentCountDown = CurrentParam.CountDown - (FPlatformTime::Seconds() - CurrentParam.RegistTime);
		CurrentShowingCountDown = FMath::CeilToInt64(CurrentCountDown);
		Message->SetMessageText(CurrentParam.CountTextType, FText::Format(CurrentParam.Format, CurrentShowingCountDown));



		switch (CurrentParam.CountTextType)
		{
		case ETextDirection::Bottom:
		{
			Message->SetMessageText(ETextDirection::Top, InParam.TopMessage);
			Message->SetMessageText(ETextDirection::Middle, InParam.MiddleMessage);
		}
			break;

		case ETextDirection::Top:
		{
			Message->SetMessageText(ETextDirection::Middle, InParam.MiddleMessage);
			Message->SetMessageText(ETextDirection::Bottom, InParam.BottomMessage);
		}
			break;

		case ETextDirection::Middle:
		{
			Message->SetMessageText(ETextDirection::Top, InParam.TopMessage);
			Message->SetMessageText(ETextDirection::Bottom, InParam.BottomMessage);
		}
			break;

		default:
		{
			Message->SetMessageText(ETextDirection::Top, InParam.TopMessage);
			Message->SetMessageText(ETextDirection::Middle, InParam.MiddleMessage);
			Message->SetMessageText(ETextDirection::Bottom, InParam.BottomMessage);
		}
			break;
		}


		if (bCustomStyle && CustomStyle.Contains(CurrentParam.MessageType))
		{
			Message->SetMessageStyle(CustomStyle[CurrentParam.MessageType]);
		}
	}

	
	SetVisibility(true == bSkipToClick ? ESlateVisibility::Visible : ESlateVisibility::HitTestInvisible);

	if (bAnimating)
	{
		CurrentDuration += FadeIn->GetEndTime();
		PlayAnimationForward(FadeIn);
	}

	FTSTicker::RemoveTicker(TickDelegateHandle);
	TickDelegateHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &UNotificationMessageUI::OnTicking), TickDelay);
}

bool UNotificationMessageUI::GetIconType(const FGameplayTag& InMessageType, OUT TSoftObjectPtr<UObject>* OutImageSource)
{
	if (bCustomStyle && CustomStyle.Contains(InMessageType))
	{
		*OutImageSource = CustomStyle[InMessageType].IconSoftPtr;
		return true;
	}

	return false;
}

void UNotificationMessageUI::OnEndedFadeOutAnimation()
{
	OnEndedNotify.ExecuteIfBound();
	NextMessage();
}


bool UNotificationMessageUI::OnTicking(float InDeltaTime)
{
	if (CurrentDuration <= FPlatformTime::Seconds())
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
		double NowCalculatedCountDown = CurrentParam.CountDown - (FPlatformTime::Seconds() - CurrentParam.RegistTime);
		int64 NowCalculatedCountDownInt = FMath::CeilToInt64(NowCalculatedCountDown);

		if (NowCalculatedCountDownInt <= 0)
		{
			CurrentShowingCountDown = 0;
			if (nullptr != Message) Message->SetMessageText(CurrentParam.CountTextType, FText::Format(CurrentParam.Format, 0));
		}

		else if (NowCalculatedCountDownInt < CurrentShowingCountDown)
		{
			CurrentShowingCountDown = NowCalculatedCountDownInt;
			if (nullptr != Message) Message->SetMessageText(CurrentParam.CountTextType, FText::Format(CurrentParam.Format, CurrentShowingCountDown));
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
		if (ensure(FadeOut))
		{
			FadeOutAnimationEvent.Clear();
			FadeOutAnimationEvent.BindDynamic(this, &UNotificationMessageUI::OnEndedFadeOutAnimation);
			BindToAnimationFinished(FadeOut, FadeOutAnimationEvent);
		}

	}

	FTSTicker::RemoveTicker(TickDelegateHandle);

	SetVisibility(ESlateVisibility::Collapsed);
	SetConsumePointerInput(true);
}

void UNotificationMessageUI::NativeDestruct()
{
	FTSTicker::RemoveTicker(TickDelegateHandle);

	if (nullptr != FadeOut && FadeOutAnimationEvent.IsBound())
	{
		UnbindAllFromAnimationFinished(FadeOut);
		FadeOutAnimationEvent.Clear();
	}


	ParamQueue.Empty();

	Super::NativeDestruct();
}

FReply UNotificationMessageUI::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InPointEvent)
{
	OnTouchDelegate.Broadcast(CurrentParam.MessageType);

	if (true == bSkipToClick)
	{
		NextMessage();
	}
	
	return FReply::Handled();
}

FReply UNotificationMessageUI::NativeOnTouchEnded(const FGeometry& InGeometry, const FPointerEvent& InPointEvent)
{
	OnTouchDelegate.Broadcast(CurrentParam.MessageType);

	if (true == bSkipToClick)
	{
		NextMessage();
	}

	return FReply::Handled();
}

void UNotificationMessageUI::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);


}

