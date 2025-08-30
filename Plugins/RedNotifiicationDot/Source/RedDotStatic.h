// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnChangedVisibleSignature, bool /* bVisible */)

class FRedDotNode
{
public:
	FRedDotNode(FGameplayTag InMyTag) : MyTag(InMyTag), SubscribeCount(0), ChildNotifyCount(0), NotifyCount(0)
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

		ParentNodes.Reset();
		ChildNodes.Reset();
	};


public:
	FOnChangedVisibleSignature OnChangedVisible;

	void On();
	void Off();
	void Notify(uint32 InCount);

	uint32 GetSubscribeCount() const { return SubscribeCount; }
	uint32 GetCount() const { return ChildNotifyCount + NotifyCount; }

	void AddParent(const TSharedPtr<FRedDotNode> InParentNode) { ParentNodes.Emplace(InParentNode); }
	void AddChild(const TSharedPtr<FRedDotNode> InChildNode) { ChildNodes.Emplace(InChildNode); }
	void RemoveParent(const TSharedPtr<FRedDotNode> InParentNode) { ParentNodes.Remove(InParentNode); }
	void RemoveChild(const TSharedPtr<FRedDotNode> InChildNode) { ChildNodes.Remove(InChildNode); }

	const TSet<TWeakPtr<FRedDotNode>>& GetParents() const { return ParentNodes; }
	const TSet<TWeakPtr<FRedDotNode>>& GetChilds() const { return ChildNodes; }

	void SetSubcribe() { ++SubscribeCount; }
	void ReleaseSubscribe() { --SubscribeCount; }

private:
	TSet<TWeakPtr<FRedDotNode>> ParentNodes {};
	TSet<TWeakPtr<FRedDotNode>> ChildNodes {};

	FGameplayTag MyTag{};

	uint32 SubscribeCount = 0;
	uint32 ChildNotifyCount = 0;
	uint32 NotifyCount = 0;

};

void FRedDotNode::Notify(uint32 InCount)
{
	bool IsOn = NotifyCount < InCount && InCount > 0 ? true : false;

	NotifyCount = InCount;

	if (false == ChildNodes.IsEmpty())
	{
		ChildNotifyCount = 0;

		for (auto& Child : ChildNodes)
		{
			if (false == Child.IsValid()) continue;

			ChildNotifyCount += Child.Pin()->GetCount();
		}
	}

	for (auto& ParentNode : ParentNodes)
	{
		if (false == ParentNode.IsValid()) continue;
		true == IsOn ? ParentNode.Pin()->On() : ParentNode.Pin()->Off();
	}

	OnChangedVisible.Broadcast(ChildNotifyCount + NotifyCount > 0);
}

void FRedDotNode::On()
{
	int Count = 0;

	if (false == ChildNodes.IsEmpty())
	{
		ChildNotifyCount = 0;

		for (auto& Child : ChildNodes)
		{
			if (false == Child.IsValid()) continue;

			ChildNotifyCount += Child.Pin()->GetCount();
		}
	}

	for (auto& ParentNode : ParentNodes)
	{
		if (false == ParentNode.IsValid()) continue;
		ParentNode.Pin()->On();
	}

	Count = ChildNotifyCount + NotifyCount;
	OnChangedVisible.Broadcast(Count > 0);
}

void FRedDotNode::Off()
{
	int Count = 0;

	if (false == ChildNodes.IsEmpty())
	{
		ChildNotifyCount = 0;

		for (auto& Child : ChildNodes)
		{
			if (false == Child.IsValid()) continue;

			ChildNotifyCount += Child.Pin()->GetCount();
		}
	}

	for (auto& ParentNode : ParentNodes)
	{
		if (false == ParentNode.IsValid()) continue;
		ParentNode.Pin()->Off();
	}

	Count = ChildNotifyCount + NotifyCount;
	OnChangedVisible.Broadcast(Count > 0);
}

