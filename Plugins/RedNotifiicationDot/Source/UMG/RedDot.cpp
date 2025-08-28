// Fill out your copyright notice in the Description page of Project Settings.


#include "UMG/RedDot.h"

#include "Components/TextBlock.h"
#include "Components/PanelWidget.h"

TMap<FGameplayTag, TSharedPtr<FRedDotNode>> URedDot::RedDotGraph;

TSharedPtr<FRedDotNode> URedDot::CreateNode(const FGameplayTag& InParentTag, const FGameplayTag& InMyTag)
{
	if (RedDotGraph.Contains(InMyTag))
	{
		// check GetNode
		return RedDotGraph[InMyTag];
	}

	TSharedPtr<FRedDotNode> RedDotNode = MakeShared<FRedDotNode>(InMyTag);
	if (RedDotNode.IsValid())
	{
		if (RedDotGraph.Contains(InParentTag))
		{
			TSharedPtr<FRedDotNode> ParentNode = RedDotGraph[InParentTag];

			if (ParentNode.IsValid())
			{
				ParentNode->AddChild(RedDotNode);
				RedDotNode->AddParent(ParentNode);
			}
		}

		if (InMyTag.IsValid())
		{
			RedDotGraph.Emplace(InMyTag, RedDotNode);
		}

		return RedDotNode;
	}

	return nullptr;
}

void URedDot::RemoveNode(const FGameplayTag& InMyTag)
{
	TSharedPtr<FRedDotNode>* RedDot = RedDotGraph.Find(InMyTag);

	if (RedDot && (*RedDot).IsValid())
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

void URedDot::OnChangedVisible(bool bIsVisible)
{
	if (false == bIsVisible)
	{
		SetVisibility(ESlateVisibility::Collapsed);
	}

	else
	{
		SetVisibility(ESlateVisibility::HitTestInvisible);
		SetCountText();
	}
}

void URedDot::On()
{
	if (IsVisible()) return;

	if (RedDotNode.IsValid())
	{
		RedDotNode->On();
	}

	else
	{
		if (ParentDotNode.IsValid())
		{
			ParentDotNode.Pin()->IncreaseCloneCount();
			ParentDotNode.Pin()->On();
		}

		SetVisibility(ESlateVisibility::HitTestInvisible);
		Count = 1;
		SetCountText();
	}

}
void URedDot::Off()
{
	if (false == IsVisible()) return;

	if (RedDotNode.IsValid())
	{
		RedDotNode->Off();
	}

	else
	{
		if (ParentDotNode.IsValid())
		{
			ParentDotNode.Pin()->DecreaseCloneCount();
			ParentDotNode.Pin()->Off();
		}

		SetVisibility(ESlateVisibility::Collapsed);
		Count = 0;
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

	if (MyTag.IsValid())
	{
		if (RedDotGraph.Contains(MyTag))
		{
			RedDotNode = RedDotGraph[MyTag];
			if (RedDotNode.IsValid()) RedDotNode->SetSubcribe();
		}

		else
		{
			RedDotNode = CreateNode(ParentTag, MyTag);
		}
	}

	else if (ParentTag.IsValid())
	{
		TSharedPtr<FRedDotNode>* ParentNode = RedDotGraph.Find(ParentTag);
		if (ParentNode && *ParentNode)
		{
			ParentDotNode = *ParentNode;
		}	
	}
	


	if (RedDotNode.IsValid())
	{
		RedDotNode->OnChangedVisible.AddUObject(this, &URedDot::OnChangedVisible);
	}

}
void URedDot::NativeDestruct()
{
	int SubscribeCount = 0;

	if (RedDotNode.IsValid())
	{
		RedDotNode->OnChangedVisible.RemoveAll(this);
		RedDotNode->ReleaseSubscribe();
		SubscribeCount = RedDotNode->GetSubscribeCount();
	}

	RedDotNode.Reset();
	ParentDotNode.Reset();

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
	if (RedDotNode.IsValid())
	{
		Count = RedDotNode->GetCount();
	}

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
