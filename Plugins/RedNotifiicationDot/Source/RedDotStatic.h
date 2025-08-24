// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

UENUM(BlueprintType)
enum class ERedDotCountPolicy : uint8
{
	ChildCount,
	VisibleCount,
};

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnChangedVisibleSignature, bool /* bVisible */, int32 /* Subscribe Count or Child Count */)

class FRedDotNode
{
public:
	FRedDotNode(FGameplayTag InMyTag, ERedDotCountPolicy InPolicy) : MyTag(InMyTag), CountPolicy(InPolicy), SubscribeCount(0), VisibleCount(0)
	{
		ParentNodes.Reset();
		ChildNodes.Reset();
	};

	~FRedDotNode() 
	{
		for (auto& ParentNode : ParentNodes)
		{
			ParentNode.Reset();
		}

		for (auto& ChildNode : ChildNodes)
		{
			ChildNode.Reset();
		}
	};


public:
	FOnChangedVisibleSignature OnChangedVisible;

	void On();
	void Off();

	void SetSubcribe() { ++SubscribeCount; }
	void ReleaseSubscribe() { --SubscribeCount; }

	uint32 GetChildCount() const { return ChildNodes.Num(); }
	uint32 GetSubscribeCount() const { return SubscribeCount; }
	bool GetVisible() const { return VisibleCount > 0; }

	void AddParent(const TSharedPtr<FRedDotNode> InParentNode) { ParentNodes.Emplace(InParentNode); }
	void AddChild(const TSharedPtr<FRedDotNode> InChildNode) { ChildNodes.Emplace(InChildNode); }
	void RemoveParent(const TSharedPtr<FRedDotNode> InParentNode) { ParentNodes.Remove(InParentNode); }
	void RemoveChild(const TSharedPtr<FRedDotNode> InChildNode) { ChildNodes.Remove(InChildNode); }

	const FGameplayTag& GetTag() const { return MyTag; }
	const TSet<TWeakPtr<FRedDotNode>>& GetParents() const { return ParentNodes; }
	const TSet<TWeakPtr<FRedDotNode>>& GetChilds() const { return ParentNodes; }

private:
	TSet<TWeakPtr<FRedDotNode>> ParentNodes {};
	TSet<TWeakPtr<FRedDotNode>> ChildNodes {};

	FGameplayTag MyTag{};
	ERedDotCountPolicy CountPolicy = ERedDotCountPolicy::ChildCount;
	uint32 SubscribeCount = 0;


	uint32 VisibleCount = 0;
};

void FRedDotNode::On()
{
	++VisibleCount;

	OnChangedVisible.Broadcast(VisibleCount > 0, ERedDotCountPolicy::ChildCount == CountPolicy ? ChildNodes.Num() : VisibleCount);

	for (auto& ParentNode : ParentNodes)
	{
		if (false == ParentNode.IsValid()) continue;
		ParentNode.Pin()->On();
	}
}
void FRedDotNode::Off()
{
	--VisibleCount;

	OnChangedVisible.Broadcast(VisibleCount > 0, ERedDotCountPolicy::ChildCount == CountPolicy ? ChildNodes.Num() : VisibleCount);

	for (auto& ParentNode : ParentNodes)
	{
		if (false == ParentNode.IsValid()) continue;
		ParentNode.Pin()->Off();
	}
}


namespace RedDotStatic
{
	static TMap<FGameplayTag, TSharedPtr<FRedDotNode>> RedDotGraph;

	static bool GetNode(const FGameplayTag& InTag, OUT TSharedPtr<FRedDotNode>* OutNode)
	{
		if (RedDotGraph.Contains(InTag))
		{
			*OutNode = RedDotGraph[InTag];
			return true;
		}

		return false;
	}

	static TSharedPtr<FRedDotNode> CreateNode(const FGameplayTag& InParentTag, const FGameplayTag& InMyTag, ERedDotCountPolicy InPolicy)
	{
		if (RedDotGraph.Contains(InMyTag))
		{
			return RedDotGraph[InMyTag];
		}

		TSharedPtr<FRedDotNode> RedDotNode = MakeShared<FRedDotNode>(InMyTag, InPolicy);
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

	static void RemoveNode(const FGameplayTag& InMyTag)
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

	static void Clear()
	{
		for (auto& [Tag, Node] : RedDotGraph)
		{
			Node.Reset();
		}

		RedDotGraph.Empty();
	}
};
