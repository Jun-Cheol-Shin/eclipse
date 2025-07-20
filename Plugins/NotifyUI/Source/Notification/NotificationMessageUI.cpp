// Fill out your copyright notice in the Description page of Project Settings.


#include "NotificationMessageUI.h"
#include "Animation/WidgetAnimation.h"

DEFINE_LOG_CATEGORY(NotityUILog);

void UNotificationMessageUI::RegistOneMessage(const FGameplayTag& InType, const FText& InMessage)
{
	RegistMessage_Internal(InType, InMessage, FText(), FText());
}

void UNotificationMessageUI::RegistTwoMessage(const FGameplayTag& InType, const FText& InMessage, const FText& InMessage_2)
{
	RegistMessage_Internal(InType, InMessage, InMessage_2, FText());
}

void UNotificationMessageUI::RegistThreeMessage(const FGameplayTag& InType, const FText& InMessage, const FText& InMessage_2, const FText& InMessage_3)
{
	RegistMessage_Internal(InType, InMessage, InMessage_2, InMessage_3);
}

void UNotificationMessageUI::RegistTopCountMessage(const FGameplayTag& InType, const FString& InFormat, double InCountDown)
{
	FNotificationParams NewParam(InType, FSlateApplication::Get().GetCurrentTime(), FTextFormat::FromString(InFormat), InCountDown);
	RegistParameter(NewParam);
}

void UNotificationMessageUI::RegistMiddleCountMessage(const FGameplayTag& InType, const FText& InTopMessage, const FString& InFormat, double InCountDown)
{
	FNotificationParams NewParam(InType, FSlateApplication::Get().GetCurrentTime(), InTopMessage, FTextFormat::FromString(InFormat), InCountDown, FText());
	RegistParameter(NewParam);
}

void UNotificationMessageUI::RegistBottomCountMessage(const FGameplayTag& InType, const FText& InTopMessage, const FText& InMiddleMessage, const FString& InFormat, double InCountDown)
{
	FNotificationParams NewParam(InType, FSlateApplication::Get().GetCurrentTime(), InTopMessage, InMiddleMessage, FTextFormat::FromString(InFormat), InCountDown);
	RegistParameter(NewParam);
}

void UNotificationMessageUI::UnregistMessage(const FGameplayTag& InTag)
{
	UnregistTagSet.Add(InTag);
}

void UNotificationMessageUI::RegistMessage_Internal(const FGameplayTag& InType, const FText& InMessage, const FText& InMessageTwo, const FText& InMessageThree)
{
	FNotificationParams NewParam;
	NewParam.MessageType = InType;
	NewParam.RegistTime = FSlateApplication::Get().GetCurrentTime();
	NewParam.TopMessage = InMessage;
	NewParam.MiddleMessage = InMessageTwo;
	NewParam.BottomMessage = InMessageThree;

	RegistParameter(NewParam);
}

void UNotificationMessageUI::RegistParameter(const FNotificationParams& InParam)
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
		CurrentDuration = FSlateApplication::Get().GetCurrentTime() + Duration;
		double CurrentCountDown = InParam.CountDown - (FSlateApplication::Get().GetCurrentTime() - InParam.RegistTime);
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

	else
	{
		OnEnededFadeInAnimation();
	}
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

	if (nullptr != FadeIn && FadeInAnimationEvent.IsBound())
	{
		UnbindAllFromAnimationFinished(FadeIn);
		FadeInAnimationEvent.Clear();
	}

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

