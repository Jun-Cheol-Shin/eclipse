// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "GameplayTagContainer.h"
#include "NotificationMessageUI.generated.h"


class UTextBlock;
class UCommonLazyImage;

UENUM(BlueprintType)
enum class ECountDownTextType : uint8
{
	Top,
	Middle,
	Bottom,
};


USTRUCT(BlueprintType)
struct FNotificationParams
{
	GENERATED_BODY()


public:
	FNotificationParams() {};
	FNotificationParams(const FGameplayTag& InType, double InRegistTime, const FText& InMessage) : MessageType(InType), RegistTime(InRegistTime), TopMessage(InMessage) {};
	FNotificationParams(const FGameplayTag& InType, double InRegistTime, const FText& InMessage, const FText& InMessage_2) : MessageType(InType), RegistTime(InRegistTime), TopMessage(InMessage), MiddleMessage(InMessage_2) {};
	FNotificationParams(const FGameplayTag& InType, double InRegistTime, const FText& InMessage, const FText& InMessage_2, const FText& InMessage_3)
		: MessageType(InType), RegistTime(InRegistTime), TopMessage(InMessage), MiddleMessage(InMessage_2), BottomMessage(InMessage_3) {};

	virtual ~FNotificationParams() {};

public:
	FGameplayTag MessageType = FGameplayTag();
	double RegistTime = 0;
	FText TopMessage = FText();
	FText MiddleMessage = FText();
	FText BottomMessage = FText();
};


UCLASS()
class NOTIFYUI_API UNotificationMessageUI : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	DECLARE_DELEGATE(FOnEndedNotifySignature)
	FOnEndedNotifySignature OnEndedNotify;


	void RegistMessage(const FNotificationParams& InParam);
	void RegistCountDownMessage(const FNotificationParams& InParam, ECountDownTextType InCountDownTextType, const FTextFormat& InFormat, double InCountDown);

private:
	void CheckIconLoading(const FNotificationParams& InParam);
	void SetNextMessage(const FNotificationParams& InParam);

	void SetMessageText(const FText& InParamText, UTextBlock* InTextBlock);
	void SetCountDownText(int64 InCountDownSec);

	bool GetIconType(const FGameplayTag& InMessageType, OUT TSoftObjectPtr<UObject>* OutImageSource);


	// Widget Variable
private:
	UPROPERTY(meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
	UTextBlock* TopMessage = nullptr;

	UPROPERTY(meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
	UTextBlock* MiddleMessage = nullptr;

	UPROPERTY(meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
	UTextBlock* BottomMessage = nullptr;

	UPROPERTY(meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
	UCommonLazyImage* Icon = nullptr;

	UPROPERTY(meta = (BindWidgetAnim, AllowPrivateAccess = "true"), Transient)
	UWidgetAnimation* FadeIn;


	UPROPERTY(meta = (BindWidgetAnim, AllowPrivateAccess = "true"), Transient)
	UWidgetAnimation* FadeOut;



public:
	UPROPERTY(EditInstanceOnly, meta = (Category = "Message Setting"))
	float Duration = 0.f;

	UPROPERTY(EditInstanceOnly, meta = (Category = "Message Setting"))
	float TickDelay = 0.f;

	UPROPERTY(EditInstanceOnly, meta = (Category = "Message Setting"))
	bool bTouchable = false;

	UPROPERTY(EditInstanceOnly, meta = (Category = "Message Setting"))
	bool bAnimating = false;


protected:
	UFUNCTION()
	void OnEndedFadeOutAnimation();

	UFUNCTION()
	void OnEnededFadeInAnimation();

	bool OnTicking(float InDeltaTime);
	void OnChangedLoadingState(bool bIsLoading);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	FWidgetAnimationDynamicEvent FadeInAnimationEvent;
	FWidgetAnimationDynamicEvent FadeOutAnimationEvent;

	FTSTicker::FDelegateHandle TickDelegateHandle;

	TQueue<FNotificationParams> ParamQueue;

	double CurrentMessageDuration = 0.f;
	double CountDown = 0;

	ECountDownTextType CountDownTextType = ECountDownTextType::Top;
	FTextFormat CountDownTextFormat = FTextFormat();
};
