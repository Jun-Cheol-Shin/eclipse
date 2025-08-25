// Fill out your copyright notice in the Description page of Project Settings.


#include "UMG/RedDot.h"

#include "Components/TextBlock.h"
#include "Components/PanelWidget.h"

TMap<FGameplayTag, TSharedPtr<FRedDotNode>> URedDot::RedDotGraph;

bool URedDot::GetNode(const FGameplayTag& InTag, OUT TSharedPtr<FRedDotNode>* OutNode)
{
	if (RedDotGraph.Contains(InTag))
	{
		*OutNode = RedDotGraph[InTag];
		return true;
	}

	return false;
}
TSharedPtr<FRedDotNode> URedDot::CreateNode(const FGameplayTag& InParentTag, const FGameplayTag& InMyTag)
{
	if (RedDotGraph.Contains(InMyTag))
	{
		return RedDotGraph[InMyTag];
	}

	TSharedPtr<FRedDotNode> RedDotNode = MakeShared<FRedDotNode>(InMyTag);
	if (RedDotNode.IsValid())
	{
		TSharedPtr<FRedDotNode> ParentNode = nullptr;
		if (true == GetNode(InParentTag, &ParentNode) && ParentNode.IsValid())
		{
			ParentNode->AddChild(RedDotNode.ToSharedRef());
			RedDotNode->AddParent(ParentNode.ToSharedRef());
		}

		RedDotGraph.Emplace(InMyTag, RedDotNode);

		return RedDotNode;
	}

	return nullptr;
}
void URedDot::RemoveNode(const FGameplayTag& InMyTag)
{
	TSharedPtr<FRedDotNode>* RedDot = RedDotGraph.Find(InMyTag);

	if (RedDot && *RedDot)
	{
		for (TWeakPtr<FRedDotNode> ParentNode : (*RedDot)->GetParents())
		{
			if (false == ParentNode.IsValid()) continue;
			ParentNode.Pin()->RemoveChild(*RedDot);
		}

		for (TWeakPtr<FRedDotNode> ChildNode : (*RedDot)->GetChilds())
		{
			if (false == ChildNode.IsValid()) continue;
			ChildNode.Pin()->RemoveParent(*RedDot);
		}

		(*RedDot).Reset();
		RedDotGraph.Remove(InMyTag);
	}
}
void URedDot::ClearGraph()
{
	for (auto& [Tag, Node] : RedDotGraph)
	{
		Node.Reset();
	}

	RedDotGraph.Empty();
}



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

	if (false == GetNode(MyTag, OUT &NewNode))
	{
		NewNode = CreateNode(ParentTag, MyTag);
	}


	if (NewNode.IsValid())
	{
		NewNode->SetSubcribe();
		NewNode->OnChangedVisible.AddUObject(this, &URedDot::OnChangedVisible);
		RedDotNode = NewNode;
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
		RemoveNode(MyTag);
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
