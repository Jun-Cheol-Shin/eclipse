// Fill out your copyright notice in the Description page of Project Settings.


#include "NLGameLayer.h"

#include "Widgets/CommonActivatableWidgetContainer.h"
#include "CommonActivatableWidget.h"

void UNLGameLayer::NativeConstruct()
{
	Super::NativeConstruct();

	if (nullptr != UIVariable.WidgetQueue)
	{
		UIVariable.WidgetQueue->OnDisplayedWidgetChanged().AddUObject(this, &UNLGameLayer::OnDisplayedWidgetChanged);
		//UIVariable.WidgetQueue->OnTransitioningChanged.AddUObject(this, &UEcpLayer::OnChangedTransitioning);

		UIVariable.WidgetQueue->SetTransitionDuration(0.f);
	}

	else if (nullptr != UIVariable.WidgetStack)
	{
		UIVariable.WidgetStack->OnDisplayedWidgetChanged().AddUObject(this, &UNLGameLayer::OnDisplayedWidgetChanged);
		//UIVariable.WidgetStack->OnTransitioningChanged.AddUObject(this, &UNLGameLayer::OnChangedTransitioning);

		UIVariable.WidgetStack->SetTransitionDuration(0.f);
	}

	OnConstruct();
}

void UNLGameLayer::NativeDestruct()
{
	OnDestruct();

	if (nullptr != UIVariable.WidgetQueue)
	{
		UIVariable.WidgetQueue->OnTransitioningChanged.RemoveAll(this);
		UIVariable.WidgetQueue->OnDisplayedWidgetChanged().RemoveAll(this);
	}

	else if (nullptr != UIVariable.WidgetStack)
	{
		UIVariable.WidgetStack->OnTransitioningChanged.RemoveAll(this);
		UIVariable.WidgetStack->OnDisplayedWidgetChanged().RemoveAll(this);
	}

	Super::NativeDestruct();
}

void UNLGameLayer::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	SetConsumePointerInput(true);
}


UCommonActivatableWidgetContainerBase* UNLGameLayer::GetActivatableWidgetContainer() const
{
	if (nullptr != UIVariable.WidgetQueue)
	{
		return UIVariable.WidgetQueue;
	}

	else if (nullptr != UIVariable.WidgetStack)
	{
		return UIVariable.WidgetStack;
	}

	return nullptr;
}

int UNLGameLayer::GetActivatedWidgetNum() const
{
	if (nullptr != UIVariable.WidgetQueue)
	{
		return UIVariable.WidgetQueue->GetNumWidgets();
	}

	else if (nullptr != UIVariable.WidgetStack)
	{
		return UIVariable.WidgetStack->GetNumWidgets();
	}

	return 0;
}

bool UNLGameLayer::IsOnlyOneWidget(UCommonActivatableWidget* InWidget)
{
	UCommonActivatableWidgetContainerBase* Container = GetActivatableWidgetContainer();
	check(Container);

	const TArray<UCommonActivatableWidget*>& WidgetList = Container->GetWidgetList();

	if (WidgetList.Num() == 1)
	{
		if (nullptr != WidgetList[0] && WidgetList[0] == InWidget)
		{
			return true;
		}
	}

	return false;
}

void UNLGameLayer::OnDisplayedWidgetChanged(UCommonActivatableWidget* InWidget)
{
	if (nullptr == InWidget) return;

	InWidget->OnDeactivated().RemoveAll(this);
	InWidget->OnDeactivated().AddWeakLambda(this, [this, InWidget]()
		{
			OnCompleteDisplayedWidgetDelegate.Broadcast(InWidget, this, false);
		});

	OnCompleteDisplayedWidgetDelegate.Broadcast(InWidget, this, true);
}

FReply UNLGameLayer::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	bTouchStart = true;

	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

FReply UNLGameLayer::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return Super::NativeOnMouseMove(InGeometry, InMouseEvent);
}

FReply UNLGameLayer::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (true == bTouchStart)
	{
		OnClick();
	}

	bTouchStart = false;
	return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
}

void UNLGameLayer::NativeOnMouseLeave(const FPointerEvent& InEvent)
{
	Super::NativeOnMouseLeave(InEvent);

	bTouchStart = false;
}

FReply UNLGameLayer::NativeOnTouchStarted(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)
{
	bTouchStart = true;
	return Super::NativeOnTouchStarted(InGeometry, InGestureEvent);
}

FReply UNLGameLayer::NativeOnTouchMoved(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)
{
	return Super::NativeOnTouchMoved(InGeometry, InGestureEvent);
}

FReply UNLGameLayer::NativeOnTouchEnded(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)
{
	if (true == bTouchStart)
	{
		OnClick();
		OnTouch();
	}

	bTouchStart = false;
	return NativeOnTouchEnded(InGeometry, InGestureEvent);
}

const TArray<UCommonActivatableWidget*>& UNLGameLayer::GetWidgetList() const
{
	if (nullptr != UIVariable.WidgetQueue)
	{
		return UIVariable.WidgetQueue->GetWidgetList();
	}


	check(UIVariable.WidgetStack);
	return UIVariable.WidgetStack->GetWidgetList();
}


// DEPRECATED
/*
void UEcpLayer::OnChangedTransitioning(UCommonActivatableWidgetContainerBase* InLayer, bool bIsTransitioning)
{
	// if use transitioning...
}*/

