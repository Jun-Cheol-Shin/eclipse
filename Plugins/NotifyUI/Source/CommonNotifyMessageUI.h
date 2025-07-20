// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "GameplayTagContainer.h"
#include "Engine/Font.h"
#include "CommonNotifyMessageUI.generated.h"

class UTextBlock;
class UCommonLazyImage;

struct FNotifyStyleData;

UENUM(BlueprintType)
enum class ETextDirection : uint8
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
			FontInfo[i].Size = 20.f;
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
class NOTIFYUI_API UCommonNotifyMessageUI : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	void SetMessageStyle(const FNotifyStyleData& InStyle);
	void SetMessageText(ETextDirection InType, const FText& InText);
	void SetLazyIcon(const TSoftObjectPtr<UObject>& InSoftPtr);
	void SetDisbleIcon();
	

private:
	UPROPERTY(meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
	UTextBlock* TopMessage = nullptr;

	UPROPERTY(meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
	UTextBlock* MiddleMessage = nullptr;

	UPROPERTY(meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
	UTextBlock* BottomMessage = nullptr;

	UPROPERTY(meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
	UCommonLazyImage* Icon = nullptr;
	
};
