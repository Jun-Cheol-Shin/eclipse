// Fill out your copyright notice in the Description page of Project Settings.


#include "UMG/RedDot.h"

#include "Components/TextBlock.h"
#include "Components/PanelWidget.h"


void URedDot::OnChangedVisible(bool bIsVisible, int32 InVisibleCount)
{
	if (false == bIsVisible)
	{
		SetVisibility(ESlateVisibility::Collapsed);
	}

	else
	{
		SetVisibility(ESlateVisibility::HitTestInvisible);
		Count = InVisibleCount;
		SetCountText();
	}
}

void URedDot::On()
{
	if (RedDotNode.IsValid())
	{
		RedDotNode.Pin()->On();
	}
}

void URedDot::Off()
{
	if (RedDotNode.IsValid())
	{
		RedDotNode.Pin()->Off();
	}
}

void URedDot::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	Count = 0;
	SetCountText();
	SetVisibility(ESlateVisibility::Collapsed);
}

void URedDot::NativeConstruct()
{
	Super::NativeConstruct();
	
	TSharedPtr<FRedDotNode> NewNode = nullptr;

	if (false == RedDotStatic::GetNode(MyTag, OUT &NewNode))
	{
		NewNode = RedDotStatic::CreateNode(ParentTag, MyTag, CountPolicy);
	}


	if (NewNode.IsValid())
	{
		NewNode->SetSubcribe();
		RedDotNode = NewNode;

		RedDotNode.Pin()->OnChangedVisible.AddUObject(this, &URedDot::OnChangedVisible);
	}
}

void URedDot::NativeDestruct()
{
	int SubscribeCount = 0;

	if (RedDotNode.IsValid())
	{
		RedDotNode.Pin()->OnChangedVisible.RemoveAll(this);
		RedDotNode.Pin()->ReleaseSubscribe();
		SubscribeCount = RedDotNode.Pin()->GetSubscribeCount();
	}

	RedDotNode.Reset();

	if (SubscribeCount <= 0)
	{
		RedDotStatic::RemoveNode(MyTag);
	}

	Super::NativeDestruct();
}
void URedDot::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	SetCountText();
}
void URedDot::SetCountText()
{
	if (nullptr != CountText && nullptr != CountText->GetParent())
	{
		CountText->GetParent()->SetVisibility(true == bShowCount && Count > 0 ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);

		if (true == bShowCount && Count > 0)
		{
			FNumberFormattingOptions Option;
			Option.SetUseGrouping(bUseGrouping);
			CountText->SetText(FText::AsNumber(Count, &Option));
		}
	}
}
