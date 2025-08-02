// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "../TooltipBehavior.h"
#include "UIGuideTooltip.generated.h"


class UCommonRichTextBlock;

USTRUCT(BlueprintType)
struct FGuideMessageParameters
{
	GENERATED_BODY()

public:
	FGuideMessageParameters() {};
	FGuideMessageParameters(ETooltipPosition InPos, const FString& InFormat, const TArray<FText>& InArguments, const FVector2D InOffset = FVector2D(0, 0)) :
		TooltipPositionType(InPos), Format(InFormat), FormatArguments(InArguments), Offset(InOffset)
	{};

	bool IsEmpty() const { return Format.IsEmpty(); }

public:
	UPROPERTY(EditAnywhere)
	ETooltipPosition TooltipPositionType = ETooltipPosition::Right;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Format = FString();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FText> FormatArguments;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D Offset = FVector2D();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bUseTitle = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "true == bUseTitle", EditConditionHides))
	FString TitleFormat = FString();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "true == bUseTitle", EditConditionHides))
	TArray<FText> TitleFormatArguments;
};


UCLASS()
class UIGUIDEMASK_API UUIGuideTooltip : public UCommonUserWidget, public ITooltipBehavior
{
	GENERATED_BODY()

public:
	void Set(const FTextFormat& InTitleFormat, const FFormatOrderedArguments& InTitleArguments, const FTextFormat& InDescFormat, const FFormatOrderedArguments& InArguments);
	void Set(const FTextFormat& InDescFormat, const FFormatOrderedArguments& InArguments);

	void SetInternal(UCommonRichTextBlock* InTextBlock, const FTextFormat& InFormat, const FFormatOrderedArguments& InArguments);

	const FVector2D GetTooltipPosition(ETooltipPosition InPosition, const FVector2D& InScreenSize, const FVector2D& InPivotPosition, const FVector2D& InPivotSize, const FVector2D& InTooltipSize = FVector2D());

private:
	UPROPERTY(meta = (BindWidget, AllowPrivateAccess = "true"))
	UCommonRichTextBlock* TitleText;

	UPROPERTY(meta = (BindWidget, AllowPrivateAccess = "true"))
	UCommonRichTextBlock* DescriptionText;
};
