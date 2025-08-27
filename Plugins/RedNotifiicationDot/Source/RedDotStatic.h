// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnChangedVisibleSignature, bool /* bVisible */)

class FRedDotNode
{
public:
	FRedDotNode(FGameplayTag InMyTag) : MyTag(InMyTag), SubscribeCount(0), NotifyCount(0)
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

	// 하위 노드가 로드되지 않았을 때, 상위 노드에 대한 카운트 세팅 함수
	void ForceOn(uint32 InCount);

	void SetSubcribe() { ++SubscribeCount; }
	void ReleaseSubscribe() { --SubscribeCount; }
	uint32 GetSubscribeCount() const { return SubscribeCount; }

	uint32 GetCount() const { return NotifyCount; }

	void AddParent(const TSharedPtr<FRedDotNode> InParentNode) { ParentNodes.Emplace(InParentNode); }
	void AddChild(const TSharedPtr<FRedDotNode> InChildNode) { ChildNodes.Emplace(InChildNode); }
	void RemoveParent(const TWeakPtr<FRedDotNode> InParentNode) { ParentNodes.Remove(InParentNode); }
	void RemoveChild(const TWeakPtr<FRedDotNode> InChildNode) { ChildNodes.Remove(InChildNode); }

	const TSet<TWeakPtr<FRedDotNode>>& GetParents() const { return ParentNodes; }
	const TSet<TWeakPtr<FRedDotNode>>& GetChilds() const { return ChildNodes; }


private:
	TSet<TWeakPtr<FRedDotNode>> ParentNodes {};
	TSet<TWeakPtr<FRedDotNode>> ChildNodes {};

	FGameplayTag MyTag{};

	// 노드를 갖고있는 구독자 수 (위젯 수)
	uint32 SubscribeCount = 0;

	// 알림 카운트 
	uint32 NotifyCount = 0;

};

void FRedDotNode::On()
{
	if (ChildNodes.IsEmpty())
	{
		NotifyCount = FMath::Clamp(NotifyCount + 1, 0, SubscribeCount);
	}

	else
	{
		NotifyCount = 0;

		for (auto& Child : ChildNodes)
		{
			if (false == Child.IsValid()) continue;
			NotifyCount += Child.Pin()->GetCount();
		}
	}


	for (auto& ParentNode : ParentNodes)
	{
		if (false == ParentNode.IsValid()) continue;
		ParentNode.Pin()->On();
	}
	
	OnChangedVisible.Broadcast(true);
}
void FRedDotNode::Off()
{
	if (ChildNodes.IsEmpty())
	{
		NotifyCount = FMath::Clamp(NotifyCount - 1, 0, SubscribeCount);
	}

	else
	{
		NotifyCount = 0;

		for (auto& Child : ChildNodes)
		{
			if (false == Child.IsValid()) continue;
			NotifyCount += Child.Pin()->GetCount();
		}
	}

	for (auto& ParentNode : ParentNodes)
	{
		if (false == ParentNode.IsValid()) continue;
		ParentNode.Pin()->Off();
	}

	OnChangedVisible.Broadcast(NotifyCount > 0);
}

void FRedDotNode::ForceOn(uint32 InCount)
{
	if (ChildNodes.IsEmpty())
	{
		NotifyCount = InCount;
	}

	else
	{
		NotifyCount = 0;

		for (auto& Child : ChildNodes)
		{
			if (false == Child.IsValid()) continue;
			NotifyCount += Child.Pin()->GetCount();
		}
	}


	for (auto& ParentNode : ParentNodes)
	{
		if (false == ParentNode.IsValid()) continue;
		ParentNode.Pin()->On();
	}

	OnChangedVisible.Broadcast(true);
}
