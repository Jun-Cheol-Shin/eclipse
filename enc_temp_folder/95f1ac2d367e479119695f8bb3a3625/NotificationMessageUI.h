// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "GameplayTagContainer.h"
#include "../CommonNotifyMessageUI.h"
#include "NotificationMessageUI.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(NotityUILog, Log, All);


class UCommonNotifyMessageUI;
class FTextFormat;

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
		: MessageType(InType), RegistTime(InRegistTime), TopMessage(InMessage), MiddleMessage(InMessage_2), BottomMessage(InMessage_3) {
	};

	// Counting Type Text
	FNotificationParams(const FGameplayTag& InType, double InRegistTime, const FTextFormat& InFormat, double InCountDownSec, const FText& InMessage = FText(), const FText& InMessage_2 = FText()) :
		MessageType(InType), RegistTime(InRegistTime), MiddleMessage(InMessage), BottomMessage(InMessage_2), CountTextType(ETextDirection::Top), Format(InFormat), CountDown(InCountDownSec)
	{
		TopMessage = FText::Format(InFormat, InCountDownSec);
	}

	FNotificationParams(const FGameplayTag& InType, double InRegistTime, const FText& InMessage, const FTextFormat& InFormat, double InCountDownSec, const FText& InMessage_2) :
		MessageType(InType), RegistTime(InRegistTime), TopMessage(InMessage), BottomMessage(InMessage_2), CountTextType(ETextDirection::Middle), Format(InFormat), CountDown(InCountDownSec)
	{

		MiddleMessage = FText::Format(InFormat, InCountDownSec);
	}

	FNotificationParams(const FGameplayTag& InType, double InRegistTime, const FText& InMessage, const FText& InMessage_2, const FTextFormat& InFormat, double InCountDownSec) :
		MessageType(InType), RegistTime(InRegistTime), TopMessage(InMessage), MiddleMessage(InMessage_2), CountTextType(ETextDirection::Bottom), Format(InFormat), CountDown(InCountDownSec)
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


	ETextDirection CountTextType = ETextDirection::None;
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

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnTouchSignature, const FGameplayTag&)
	FOnTouchSignature OnTouchDelegate;

public:
	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	void RegistOneMessage(const FGameplayTag& InType, const FText& InMessage);

	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	void RegistTwoMessage(const FGameplayTag& InType, const FText& InMessage, const FText& InMessage_2);

	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	void RegistThreeMessage(const FGameplayTag& InType, const FText& InMessage, const FText& InMessage_2, const FText& InMessage_3);

	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	void RegistTopCountMessage(const FGameplayTag& InType, const FString& InFormat, double InCountDown);

	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	void RegistMiddleCountMessage(const FGameplayTag& InType, const FText& InTopMessage, const FString& InFormat, double InCountDown);

	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	void RegistBottomCountMessage(const FGameplayTag& InType, const FText& InTopMessage, const FText& InMiddleMessage, const FString& InFormat, double InCountDown);

	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	void UnregistMessage(const FGameplayTag& InTag);

private:
	UPROPERTY(EditInstanceOnly, meta = (Category = "Message Setting", AllowPrivateAccess = "true"))
	float Duration = 0.f;

	UPROPERTY(EditInstanceOnly, meta = (Category = "Message Setting", AllowPrivateAccess = "true", ClampMin = "0", ClampMax = "10"))
	int64 MaximumQueueSize = 10;

	UPROPERTY(EditInstanceOnly, meta = (Category = "Message Setting", AllowPrivateAccess = "true"))
	bool bSkipToClick = false;

	UPROPERTY(EditInstanceOnly, meta = (Category = "Message Setting", AllowPrivateAccess = "true"))
	bool bAnimating = false;

	UPROPERTY(EditDefaultsOnly, meta = (Category = "Message Setting", AllowPrivateAccess = "true"))
	float TickDelay = 0.f;

	UPROPERTY(EditDefaultsOnly, meta = (Category = "Message Setting", AllowPrivateAccess = "true"))
	bool bCustomStyle = false;

	UPROPERTY(EditDefaultsOnly, Transient, meta = (Category = "Message Setting", AllowPrivateAccess = "true", EditCondition = "true == bCustomStyle", EditConditionHides))
	TMap<FGameplayTag, FNotifyStyleData> CustomStyle;

private:
	void RegistMessage_Internal(const FGameplayTag& InType, const FText& InMessage, const FText& InMessageTwo, const FText& InMessageThree);
	void RegistParameter(const FNotificationParams& InParam);

private:
	void NextMessage();
	void SetMessage(const FNotificationParams& InParam);

	bool GetIconType(const FGameplayTag& InMessageType, OUT TSoftObjectPtr<UObject>* OutImageSource);

	// Widget Variable
private:
	UPROPERTY(meta = (BindWidgetAnim, AllowPrivateAccess = "true"), Transient)
	UWidgetAnimation* FadeIn;

	UPROPERTY(meta = (BindWidgetAnim, AllowPrivateAccess = "true"), Transient)
	UWidgetAnimation* FadeOut;

	UPROPERTY(meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
	UCommonNotifyMessageUI* Message;


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
	virtual FReply NativeOnTouchEnded(const FGeometry& InGeometry, const FPointerEvent& InPointEvent) override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

private:
	FWidgetAnimationDynamicEvent FadeInAnimationEvent;
	FWidgetAnimationDynamicEvent FadeOutAnimationEvent;

	FTSTicker::FDelegateHandle TickDelegateHandle;

	TQueue<FNotificationParams> ParamQueue;
	TSet<FGameplayTag> UnregistTagSet;

	double CurrentDuration = 0.f;


	FNotificationParams CurrentParam;
	int64 CurrentShowingCountDown = 0;
	
	int64 CurrentQueueSize = 0;
};
