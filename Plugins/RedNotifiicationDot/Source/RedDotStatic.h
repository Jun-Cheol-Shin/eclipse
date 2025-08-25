// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnChangedVisibleSignature, bool /* bVisible */, int32 /* Notify Count */)

class FRedDotNode
{
public:
	FRedDotNode(FGameplayTag InMyTag) : MyTag(InMyTag), SubscribeCount(0), NotifyCount(0), ForceCount(0)
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

	// 하위 노드가 로드되지 않았을 때, 상위 노드에 대한 카운트 세팅 함수
	void ForceOn(uint32 InCount);

	void SetSubcribe() { ++SubscribeCount; }
	void ReleaseSubscribe() { --SubscribeCount; }
	uint32 GetSubscribeCount() const { return SubscribeCount; }

	void AddParent(const TSharedPtr<FRedDotNode> InParentNode) { ParentNodes.Emplace(InParentNode); }
	void AddChild(const TSharedPtr<FRedDotNode> InChildNode) { ChildNodes.Emplace(InChildNode); }
	void RemoveParent(const TSharedPtr<FRedDotNode> InParentNode) { ParentNodes.Remove(InParentNode); }
	void RemoveChild(const TSharedPtr<FRedDotNode> InChildNode) { ChildNodes.Remove(InChildNode); }

	const TSet<TWeakPtr<FRedDotNode>>& GetParents() const { return ParentNodes; }
	const TSet<TWeakPtr<FRedDotNode>>& GetChilds() const { return ParentNodes; }

private:
	void Notify(bool bIsOn);

private:
	TSet<TWeakPtr<FRedDotNode>> ParentNodes {};
	TSet<TWeakPtr<FRedDotNode>> ChildNodes {};

	FGameplayTag MyTag{};

	// 같은 태그 노드를 공유하는 구독자 수
	uint32 SubscribeCount = 0;

	// 자식 알림 카운트 합
	uint32 NotifyCount = 0;

	// 강제 세팅 카운트
	uint32 ForceCount = 0;

};

void FRedDotNode::Notify(bool bIsOn)
{
	if (true == bIsOn)
	{
		++NotifyCount;
		if (ForceCount < NotifyCount) ForceCount = NotifyCount;
	}

	else
	{
		--NotifyCount;
		if (ForceCount > NotifyCount) ForceCount = NotifyCount;
	}
	
	for (auto& ParentNode : ParentNodes)
	{
		if (false == ParentNode.IsValid()) continue;
		ParentNode.Pin()->Notify(bIsOn);
	}

	OnChangedVisible.Broadcast(NotifyCount > 0, NotifyCount);
}

void FRedDotNode::On()
{
	for (auto& ParentNode : ParentNodes)
	{
		if (false == ParentNode.IsValid()) continue;
		ParentNode.Pin()->Notify(true);
	}

	OnChangedVisible.Broadcast(true, 1);
}
void FRedDotNode::Off()
{
	for (auto& ParentNode : ParentNodes)
	{
		if (false == ParentNode.IsValid()) continue;
		ParentNode.Pin()->Notify(false);
	}

	OnChangedVisible.Broadcast(false, 0);
}

void FRedDotNode::ForceOn(uint32 InCount)
{
	ForceCount = InCount;

	for (auto& ParentNode : ParentNodes)
	{
		if (false == ParentNode.IsValid()) continue;
		ParentNode.Pin()->ForceOn(ForceCount);
	}

	OnChangedVisible.Broadcast(ForceCount > 0, ForceCount);
}
