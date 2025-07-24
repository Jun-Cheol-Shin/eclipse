// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonNotifyMessageUI.h"
#include "ToastMessageTextUI.generated.h"

class USizeBox;
enum class EToastStackType : uint8;

UENUM(BlueprintType)
enum class EAnimationType : uint8
{
	FadeIn_Veritcal,
	FadeIn_Horizontal,
};

USTRUCT()
struct FToastMessageData
{
	GENERATED_BODY()

public:
	FToastMessageData() {};
	FToastMessageData(const FGameplayTag& InTag, const FTextFormat& InFormat, FFormatOrderedArguments InArguments) : MessageType(InTag), Format(InFormat), Arguments(InArguments) {};
	FToastMessageData(const FGameplayTag& InTag, const FText& InMessageText) : MessageType(InTag), MessageText(InMessageText) {};
	~FToastMessageData() {};

	bool IsPossibleMerge(const FToastMessageData& InData)
	{
		if (false == MessageType.MatchesTagExact(InData.MessageType))
		{
			return false;
		}

		if (false == Format.GetSourceString().Equals(InData.Format.GetSourceString()))
		{
			return false;
		}
		if (Arguments.Num() != InData.Arguments.Num())
		{
			return false;
		}

		if (false == MessageText.IsEmpty())
		{
			return false;
		}

		return true;
	}

	void Merge(const FToastMessageData& InData)
	{
		for (int i = 0; i < Arguments.Num(); ++i)
		{
			if (EFormatArgumentType::Int == InData.Arguments[i].GetType() && EFormatArgumentType::Int == this->Arguments[i].GetType())
			{
				int64 Sum = InData.Arguments[i].GetIntValue() + this->Arguments[i].GetIntValue();
				this->Arguments[i] = FFormatArgumentValue(Sum);
			}

			else if (EFormatArgumentType::UInt == InData.Arguments[i].GetType() && EFormatArgumentType::UInt == this->Arguments[i].GetType())
			{
				uint64 Sum = InData.Arguments[i].GetUIntValue() + this->Arguments[i].GetUIntValue();
				this->Arguments[i] = FFormatArgumentValue(Sum);
			}

			else if (EFormatArgumentType::Float == InData.Arguments[i].GetType() && EFormatArgumentType::Float == this->Arguments[i].GetType())
			{
				float Sum = InData.Arguments[i].GetFloatValue() + this->Arguments[i].GetFloatValue();
				this->Arguments[i] = FFormatArgumentValue(Sum);
			}

			else if (EFormatArgumentType::Double == InData.Arguments[i].GetType() && EFormatArgumentType::Double == this->Arguments[i].GetType())
			{
				double Sum = InData.Arguments[i].GetDoubleValue() + this->Arguments[i].GetDoubleValue();
				this->Arguments[i] = FFormatArgumentValue(Sum);
			}
		}
	}

	bool IsValid() const
	{
		return true == MessageType.IsValid() && (false == MessageText.IsEmpty() || false == Format.GetSourceString().IsEmpty());
	}

public:
	FGameplayTag MessageType;
	FText MessageText;
	FTextFormat Format;
	FFormatOrderedArguments Arguments;

};

UCLASS()
class NOTIFYUI_API UToastMessageTextUI : public UCommonNotifyMessageUI
{
	GENERATED_BODY()

public:
	DECLARE_DELEGATE_TwoParams(FOnCompleteAnimEndSignature, UToastMessageTextUI*, const FGameplayTag&)
	FOnCompleteAnimEndSignature OnCompleteAnimEndDelegate;

	void Start(const FToastMessageData& InData, bool bReversed = false);
	void End();
	
	void SetDirection(bool bReversed = false);

	bool IsEqual(const FGameplayTag& InTag) const { return MessageData.MessageType.MatchesTagExact(InTag); }

private:
	void PlayToastAnimation();

private:
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	EAnimationType AnimationType;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void RemoveFromParent() override;

private:
	FToastMessageData MessageData {};

private:
	UFUNCTION()
	void OnCompleteFadeInAnimation();
	
	// Widget Variable
private:
	UPROPERTY(meta = (BindWidgetAnim, AllowPrivateAccess = "true"), Transient)
	UWidgetAnimation* Up;

	UPROPERTY(meta = (BindWidgetAnim, AllowPrivateAccess = "true"), Transient)
	UWidgetAnimation* Down;

	UPROPERTY(meta = (BindWidgetAnim, AllowPrivateAccess = "true"), Transient)
	UWidgetAnimation* Left;

	UPROPERTY(meta = (BindWidgetAnim, AllowPrivateAccess = "true"), Transient)
	UWidgetAnimation* Right;
	
	UPROPERTY(meta = (BindWidgetAnim, AllowPrivateAccess = "true"), Transient)
	UWidgetAnimation* FadeOut;

	UPROPERTY(meta = (BindWidget, AllowPrivateAccess = "true"))
	USizeBox* SizeBox;

};
