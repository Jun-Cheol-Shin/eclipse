// Fill out your copyright notice in the Description page of Project Settings.


#include "UMG/RedDot.h"

#include "Components/TextBlock.h"
#include "Components/PanelWidget.h"

TMap<FGameplayTag, TSharedPtr<FRedDotNode>> URedDot::RedDotGraph;

TSharedPtr<FRedDotNode> URedDot::CreateNode(const FGameplayTag& InMyTag)
{
	if (RedDotGraph.Contains(InMyTag))
	{
		// check GetNode
		return RedDotGraph[InMyTag];
	}

	TSharedPtr<FRedDotNode> RedDotNode = MakeShared<FRedDotNode>(InMyTag);
	if (RedDotNode.IsValid())
	{
		/*if (RedDotGraph.Contains(InParentTag))
		{
			TSharedPtr<FRedDotNode> ParentNode = RedDotGraph[InParentTag];

			if (ParentNode.IsValid())
			{
				ParentNode->AddChild(RedDotNode);
				RedDotNode->AddParent(ParentNode);
			}
		}*/

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

	else if (false == RedDotNode.IsValid()) return;


	// 주체가 되는 레드닷 일 경우..
	if (true == bEnableIncludeChild)
	{
		RedDotNode.Pin()->On();
	}

	// 자식 레드닷일 경우 (주체가 되지 않음)
	else
	{
		SetVisibility(ESlateVisibility::HitTestInvisible);
		int ParentNotifyCount = RedDotNode.Pin()->GetCount();
		RedDotNode.Pin()->Notify(ParentNotifyCount + 1);
	}

}
void URedDot::Off()
{
	if (false == IsVisible()) return;

	// 주체가 되는 레드닷 일 경우..
	if (true == bEnableIncludeChild)
	{
		RedDotNode.Pin()->Off();
	}

	// 자식 레드닷일 경우 (주체가 되지 않음)
	else
	{
		SetVisibility(ESlateVisibility::Collapsed);
		int ParentNotifyCount = RedDotNode.Pin()->GetCount();
		RedDotNode.Pin()->Notify(ParentNotifyCount - 1);
	}
}

void URedDot::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	SetVisibility(ESlateVisibility::Collapsed);
}

void URedDot::NativeConstruct()
{
	Super::NativeConstruct();

	if (true == bEnableIncludeChild)
	{
		TSharedPtr<FRedDotNode> MyNode = nullptr;
		TSharedPtr<FRedDotNode> ParentNode = nullptr;

		if (MyTag.IsValid())
		{
			MyNode = RedDotGraph.Contains(MyTag) ? RedDotGraph[MyTag] : CreateNode(MyTag);

			if (ensure(MyNode.IsValid()))
			{
				MyNode->SetSubcribe();
				MyNode->OnChangedVisible.AddUObject(this, &URedDot::OnChangedVisible);
				RedDotNode = MyNode;
			}
		}


		if (ParentTag.IsValid())
		{
			ParentNode = RedDotGraph.Contains(ParentTag) ? RedDotGraph[ParentTag] : CreateNode(ParentTag);

			if (ensure(ParentNode.IsValid()))
			{
				// 부모 노드를 찾았을 때, 부모 관계를 연결해준다.
				ParentNode->AddChild(RedDotNode.Pin());

				// MyNode가 없을 수 있을까?
				if (MyNode.IsValid()) MyNode->AddParent(ParentNode);
			}
		}
	}

	else
	{
		TSharedPtr<FRedDotNode> ParentNode = nullptr;
		if (ParentTag.IsValid())
		{
			ParentNode = RedDotGraph.Contains(ParentTag) ? RedDotGraph[ParentTag] : CreateNode(ParentTag);

			if (ensure(ParentNode.IsValid()))
			{
				RedDotNode = ParentNode;
			}
		}
	}
}

void URedDot::NativeDestruct()
{
	int SubscribeCount = 0;
	if (RedDotNode.IsValid())
	{
		if (true == bEnableIncludeChild)
		{
			RedDotNode.Pin()->OnChangedVisible.RemoveAll(this);

			RedDotNode.Pin()->Off();
			RedDotNode.Pin()->ReleaseSubscribe();
			SubscribeCount = RedDotNode.Pin()->GetSubscribeCount();

			if (SubscribeCount <= 0)
			{
				RemoveNode(MyTag);
			}
		}

		else
		{
			int ParentNotifyCount = RedDotNode.Pin()->GetCount();
			RedDotNode.Pin()->Notify(ParentNotifyCount - 1);
		}
	}

	RedDotNode.Reset();

	Super::NativeDestruct();
}
void URedDot::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	if (IsDesignTime())
	{
		if (nullptr != CountText && nullptr != CountText->GetParent())
		{
			CountText->GetParent()->SetVisibility(true == bShowCount && PreviewCount > 0 ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);

			if (true == bShowCount && PreviewCount > 0)
			{
				FNumberFormattingOptions Option;
				Option.SetUseGrouping(bUseGrouping);
				CountText->SetText(FText::AsNumber(PreviewCount, &Option));
			}
		}
	}
}
void URedDot::SetCountText()
{
	if (false == bEnableIncludeChild) return;

	int Count = RedDotNode.IsValid() ? RedDotNode.Pin()->GetCount() : 0;

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
