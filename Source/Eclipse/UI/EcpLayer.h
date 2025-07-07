// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "EcpLayer.generated.h"

/**
 * 
 */
UCLASS()
class ECLIPSE_API UEcpLayer : public UCommonActivatableWidget
{
	GENERATED_BODY()

protected:
	virtual void OnClick() {};
	
	// Called after widget setted
	virtual void OnConstruct() {};
	
private:
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	
	virtual void NativeOnMouseLeave(const FPointerEvent& InEvent) override;

	/*
	virtual FReply NativeOnTouchStarted(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent) override;
	virtual FReply NativeOnTouchMoved(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent) override;
	virtual FReply NativeOnTouchEnded(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent) override;
	*/

private:
	virtual void NativeConstruct() override;
	virtual void NativeOnInitialized() override;

private:
	bool bTouchStart = false;
	
};
