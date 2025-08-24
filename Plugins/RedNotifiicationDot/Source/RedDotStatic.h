// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

UENUM(BlueprintType)
enum class ERedDotCountPolicy : uint8
{
	ChildNodes,
	Subscribers,
};

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnChangedVisibleSignature, bool /* bVisible */, int32 /* Subscribe Count or Child Count */)

class FRedDotNode
{
public:
	FRedDotNode(FGameplayTag InMyTag, ERedDotCountPolicy InPolicy) : MyTag(InMyTag), CountPolicy(InPolicy), SubscribeCount(0), IsVisible(false)
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

	void On()
	{
		IsVisible = true;
		int32 ChildSubscribeCount = 0;

		for (auto& Child : ChildNodes)
		{
			if (false == Child.IsValid()) continue;
			ChildSubscribeCount += Child.Pin()->GetSubscribeCount();
		}

		OnChangedVisible.Broadcast(IsVisible, ERedDotCountPolicy::ChildNodes == CountPolicy ? ChildNodes.Num() : ChildSubscribeCount);

		for (auto& ParentNode : ParentNodes)
		{
			if (false == ParentNode.IsValid()) continue;
			ParentNode.Pin()->On();
		}
	}

	void Off()
	{
		bool bVisibleChild = false;
		int32 ChildSubscribeCount = 0;

		for (auto& Child : ChildNodes)
		{
			if (false == Child.IsValid()) continue;

			if (Child.IsValid() && Child.Pin()->GetVisible() == true)
			{
				bVisibleChild = true;
			}

			ChildSubscribeCount += Child.Pin()->GetSubscribeCount();
		}

		IsVisible = bVisibleChild;
		OnChangedVisible.Broadcast(IsVisible, ERedDotCountPolicy::ChildNodes == CountPolicy ? ChildNodes.Num() : ChildSubscribeCount);

		for (auto& ParentNode : ParentNodes)
		{
			if (false == ParentNode.IsValid()) continue;
			ParentNode.Pin()->Off();
		}
	}

	void SetSubcribe() { ++SubscribeCount; }
	void ReleaseSubscribe() { if (SubscribeCount > 0) --SubscribeCount; }

	const FGameplayTag& GetTag() const { return MyTag; }
	int32 GetSubscribeCount() const { return SubscribeCount; }
	bool GetVisible() const { return IsVisible; }

	void AddParent(const TSharedPtr<FRedDotNode> InParentNode) { ParentNodes.Emplace(InParentNode); }
	void AddChild(const TSharedPtr<FRedDotNode> InChildNode) { ChildNodes.Emplace(InChildNode); }

private:
	TSet<TWeakPtr<FRedDotNode>> ParentNodes {};
	TSet<TWeakPtr<FRedDotNode>> ChildNodes {};

	FGameplayTag MyTag{};
	ERedDotCountPolicy CountPolicy = ERedDotCountPolicy::ChildNodes;
	int32 SubscribeCount = 0;
	bool IsVisible = false;
};


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
