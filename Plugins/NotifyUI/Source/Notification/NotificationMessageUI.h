// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "GameplayTagContainer.h"
#include "NotificationMessageUI.generated.h"


class UTextBlock;
class UCommonLazyImage;
class FTextFormat;

UENUM(BlueprintType)
enum class ECountDownTextDirection : uint8
{
	None,
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

	// Normal Type Text
	FNotificationParams(const FGameplayTag& InType, double InRegistTime, const FText& InMessage) : MessageType(InType), RegistTime(InRegistTime), TopMessage(InMessage) {};
	FNotificationParams(const FGameplayTag& InType, double InRegistTime, const FText& InMessage, const FText& InMessage_2) : MessageType(InType), RegistTime(InRegistTime), TopMessage(InMessage), MiddleMessage(InMessage_2) {};
	FNotificationParams(const FGameplayTag& InType, double InRegistTime, const FText& InMessage, const FText& InMessage_2, const FText& InMessage_3)
		: MessageType(InType), RegistTime(InRegistTime), TopMessage(InMessage), MiddleMessage(InMessage_2), BottomMessage(InMessage_3) {};

	// Counting Type Text
	FNotificationParams(const FGameplayTag& InType, double InRegistTime, const FTextFormat& InFormat, double InCountDownSec, const FText& InMessage = FText(), const FText& InMessage_2 = FText()) : 
		MessageType(InType), RegistTime(InRegistTime), CountTextType(ECountDownTextDirection::Top), Format(InFormat), CountDown(InCountDownSec), MiddleMessage(InMessage), BottomMessage(InMessage_2)
	{
		TopMessage = FText::Format(InFormat, InCountDownSec);
	}

	FNotificationParams(const FGameplayTag& InType, double InRegistTime, const FText& InMessage, const FTextFormat& InFormat, double InCountDownSec, const FText& InMessage_2) :
		MessageType(InType), RegistTime(InRegistTime), CountTextType(ECountDownTextDirection::Middle), Format(InFormat), CountDown(InCountDownSec), TopMessage(InMessage), BottomMessage(InMessage_2)
	{

		MiddleMessage = FText::Format(InFormat, InCountDownSec);
	}

	FNotificationParams(const FGameplayTag& InType, double InRegistTime, const FText& InMessage, const FText& InMessage_2, const FTextFormat& InFormat, double InCountDownSec) :
		MessageType(InType), RegistTime(InRegistTime), CountTextType(ECountDownTextDirection::Bottom), Format(InFormat), CountDown(InCountDownSec), TopMessage(InMessage), MiddleMessage(InMessage_2)
	{
		BottomMessage = FText::Format(InFormat, InCountDownSec);
	}



	virtual ~FNotificationParams() {};

public:
	FGameplayTag MessageType = FGameplayTag();
	double RegistTime = 0;
	FText TopMessage = FText();
	FText MiddleMessage = FText();
	FText BottomMessage = FText();


	ECountDownTextDirection CountTextType = ECountDownTextDirection::None;
	FTextFormat Format = FTextFormat();
	double CountDown = 0.f;
};


UCLASS()
class NOTIFYUI_API UNotificationMessageUI : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	DECLARE_DELEGATE(FOnEndedNotifySignature)
	FOnEndedNotifySignature OnEndedNotify;

	DECLARE_DELEGATE_OneParam(FOnTouchSignature, const FGameplayTag&)
	FOnEndedNotifySignature OnTouchDelegate;

	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	void RegistMessage(const FGameplayTag& InType, const FText& InMessage);

	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	void RegistTwoMessage(const FGameplayTag& InType, const FText& InMessage, const FText& InMessageTwo);

	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	void RegistThreeMessage(const FGameplayTag& InType, const FText& InMessage, const FText& InMessageTwo, const FText& InMessageThree);

	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	void UnregistMessage(const FGameplayTag& InTag);

	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	void RegistCountDownMessage(const FGameplayTag& InType, const FString& InFormat, double InCountDown, const FText& InMessage = FText(), const FText& InMessageTwo = FText());

private:
	void RegistMessage(const FNotificationParams& InParam);

private:
	void NextMessage();
	void SetMessage(const FNotificationParams& InParam);

	void SetMessageText(const FText& InParamText, UTextBlock* InTextBlock);
	bool GetIconType(const FGameplayTag& InMessageType, OUT TSoftObjectPtr<UObject>* OutImageSource);

	void SetCountdownMessage(int64 InShowCountDownSec);


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

	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InPointEvent) override;

private:
	FWidgetAnimationDynamicEvent FadeInAnimationEvent;
	FWidgetAnimationDynamicEvent FadeOutAnimationEvent;

	FTSTicker::FDelegateHandle TickDelegateHandle;

	TQueue<FNotificationParams> ParamQueue;
	TSet<FGameplayTag> UnregistTagSet;

	double CurrentDuration = 0.f;


	FNotificationParams CurrentParam;
	int64 CurrentShowingCountDown = 0;
};
