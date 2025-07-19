// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "Engine/Font.h"
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
struct FNotifyStyleData
{
	GENERATED_BODY()
	
public:
	FNotifyStyleData()
	{
		for (int i = 0; i < 3; ++i)
		{
			FontInfo[i].FontObject = LoadObject<UObject>(nullptr, TEXT("/Engine/EngineFonts/Roboto.Roboto"));
			FontInfo[i].Size = 20;
		}
	};
	virtual ~FNotifyStyleData() {};

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, EditFixedSize)
	TArray<FLinearColor> TextColor  
	{
		FLinearColor::White,
		FLinearColor::White,
		FLinearColor::White
	};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UTexture> IconSoftPtr = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, EditFixedSize)
	TArray<FSlateFontInfo> FontInfo =
	{
		FSlateFontInfo(),
		FSlateFontInfo(),
		FSlateFontInfo(),
	};
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
		MessageType(InType), RegistTime(InRegistTime), MiddleMessage(InMessage), BottomMessage(InMessage_2), CountTextType(ECountDownTextDirection::Top), Format(InFormat), CountDown(InCountDownSec)
	{
		TopMessage = FText::Format(InFormat, InCountDownSec);
	}

	FNotificationParams(const FGameplayTag& InType, double InRegistTime, const FText& InMessage, const FTextFormat& InFormat, double InCountDownSec, const FText& InMessage_2) :
		MessageType(InType), RegistTime(InRegistTime), TopMessage(InMessage), BottomMessage(InMessage_2), CountTextType(ECountDownTextDirection::Middle), Format(InFormat), CountDown(InCountDownSec)
	{

		MiddleMessage = FText::Format(InFormat, InCountDownSec);
	}

	FNotificationParams(const FGameplayTag& InType, double InRegistTime, const FText& InMessage, const FText& InMessage_2, const FTextFormat& InFormat, double InCountDownSec) :
		MessageType(InType), RegistTime(InRegistTime), TopMessage(InMessage), MiddleMessage(InMessage_2), CountTextType(ECountDownTextDirection::Bottom), Format(InFormat), CountDown(InCountDownSec)
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

	

protected:
	void SetMessageStyle(const FNotifyStyleData& InStyle);

private:
	void RegistMessage_Internal(const FGameplayTag& InType, const FText& InMessage, const FText& InMessageTwo, const FText& InMessageThree);
	void RegistParameter(const FNotificationParams& InParam);

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
};
