// Fill out your copyright notice in the Description page of Project Settings.


#include "EcpLayer.h"

#include "Widgets/CommonActivatableWidgetContainer.h"

void UEcpLayer::NativeConstruct()
{
	Super::NativeConstruct();

	if (nullptr != UIVariable.WidgetQueue)
	{
		UIVariable.WidgetQueue->OnDisplayedWidgetChanged().AddUObject(this, &UEcpLayer::OnDisplayedWidgetChanged);
		UIVariable.WidgetQueue->OnTransitioningChanged.AddUObject(this, &UEcpLayer::OnChangedTransitioning);
	}

	else if (nullptr != UIVariable.WidgetStack)
	{
		UIVariable.WidgetStack->OnDisplayedWidgetChanged().AddUObject(this, &UEcpLayer::OnDisplayedWidgetChanged);
		UIVariable.WidgetStack->OnTransitioningChanged.AddUObject(this, &UEcpLayer::OnChangedTransitioning);
	}

	OnConstruct();
}

void UEcpLayer::NativeDestruct()
{
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

void UEcpLayer::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	SetConsumePointerInput(true);
}


UCommonActivatableWidgetContainerBase* UEcpLayer::GetActivatableWidgetContainer() const
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

void UEcpLayer::OnDisplayedWidgetChanged(UCommonActivatableWidget* InWidget)
{
	if (nullptr == InWidget) return;

	OnCompleteDisplayedWidget.ExecuteIfBound(InWidget, this);
}

void UEcpLayer::OnChangedTransitioning(UCommonActivatableWidgetContainerBase* InLayer, bool bIsTransitioning)
{
	// todo

}

FReply UEcpLayer::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	bTouchStart = true;

	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

FReply UEcpLayer::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return Super::NativeOnMouseMove(InGeometry, InMouseEvent);
}

FReply UEcpLayer::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (true == bTouchStart)
	{
		OnClick();
	}

	bTouchStart = false;
	return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
}

void UEcpLayer::NativeOnMouseLeave(const FPointerEvent& InEvent)
{
	Super::NativeOnMouseLeave(InEvent);

	bTouchStart = false;
}



/*
FReply UEcpLayer::NativeOnTouchStarted(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)
{
	return Super::NativeOnTouchStarted(InGeometry, InGestureEvent);
}

FReply UEcpLayer::NativeOnTouchMoved(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)
{

	return Super::NativeOnTouchMoved(InGeometry, InGestureEvent);
}

FReply UEcpLayer::NativeOnTouchEnded(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)
{
	if (true == bTouchStart)
	{

	}

	return NativeOnTouchEnded(InGeometry, InGestureEvent);
}*/

