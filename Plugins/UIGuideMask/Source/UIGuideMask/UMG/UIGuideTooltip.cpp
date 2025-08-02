// Fill out your copyright notice in the Description page of Project Settings.


#include "UIGuideTooltip.h"
#include "CommonRichTextBlock.h"

void UUIGuideTooltip::Set(const FTextFormat& InTitleFormat, const FFormatOrderedArguments& InTitleArguments, const FTextFormat& InDescFormat, const FFormatOrderedArguments& InArguments)
{
	TitleText->SetVisibility(ESlateVisibility::HitTestInvisible);
	SetInternal(TitleText, InTitleFormat, InTitleArguments);
	SetInternal(DescriptionText, InDescFormat, InArguments);
}

void UUIGuideTooltip::Set(const FTextFormat& InDescFormat, const FFormatOrderedArguments& InArguments)
{
	TitleText->SetVisibility(ESlateVisibility::Collapsed);
	DescriptionText->SetVisibility(ESlateVisibility::HitTestInvisible);

	SetInternal(DescriptionText, InDescFormat, InArguments);
}

void UUIGuideTooltip::SetInternal(UCommonRichTextBlock* InTextBlock, const FTextFormat& InFormat, const FFormatOrderedArguments& InArguments)
{
	if (nullptr != InTextBlock)
	{
		InTextBlock->SetText(FText::Format(InFormat, InArguments));
	}
}

const FVector2D UUIGuideTooltip::GetTooltipPosition(ETooltipPosition InPosition, const FVector2D& InScreenSize, const FVector2D& InPivotPosition, const FVector2D& InPivotSize, const FVector2D& InTooltipSize)
{
	FVector2D Location;

	if (InTooltipSize.IsZero())
	{
		GetPosition(OUT Location, InPosition, InScreenSize, InPivotPosition, InPivotSize, GetCachedWidget()->GetDesiredSize());
	}

	else
	{
		GetPosition(OUT Location, InPosition, InScreenSize, InPivotPosition, InPivotSize, InTooltipSize);
	}

	return Location;
}
