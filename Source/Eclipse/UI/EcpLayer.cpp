// Fill out your copyright notice in the Description page of Project Settings.


#include "EcpLayer.h"


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

void UEcpLayer::NativeConstruct()
{
	Super::NativeConstruct();

	OnConstruct();
}

void UEcpLayer::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	SetConsumePointerInput(true);
}


// todo
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
}
*/