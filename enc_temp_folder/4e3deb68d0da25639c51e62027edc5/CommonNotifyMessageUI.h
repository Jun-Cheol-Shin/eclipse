// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "GameplayTagContainer.h"
#include "Engine/Font.h"
#include "CommonNotifyMessageUI.generated.h"

class UCommonRichTextBlock;
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
struct FCommonMessageUIVariable
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	UCommonRichTextBlock* TopMessage = nullptr;

	UPROPERTY(BlueprintReadWrite)
	UCommonRichTextBlock* MiddleMessage = nullptr;

	UPROPERTY(BlueprintReadWrite)
	UCommonRichTextBlock* BottomMessage = nullptr;

	UPROPERTY(BlueprintReadWrite)
	UCommonLazyImage* Icon = nullptr;

};



UCLASS()
class NOTIFYUI_API UCommonNotifyMessageUI : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	void SetMessageText(const FText& InText, const FText& InText_2 = FText(), const FText& InText_3 = FText());

	void SetMessageText(ETextDirection InType, const FText& InText);

	void SetMessageStyle(const FNotifyStyleData& InStyle);
	void SetLazyIcon(const TSoftObjectPtr<UObject>& InSoftPtr);
	void SetDisbleIcon();
	

protected:
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FCommonMessageUIVariable UIVariable;
	
};
