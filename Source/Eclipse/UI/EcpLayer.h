// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "EcpLayer.generated.h"


class UCommonActivatableWidget;
class UCommonActivatableWidgetStack;
class UCommonActivatableWidgetQueue;
class UCommonActivatableWidgetContainerBase;


USTRUCT(BlueprintType)
struct FEcpLayerUIVariable
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UCommonActivatableWidgetQueue> WidgetQueue = nullptr;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UCommonActivatableWidgetStack> WidgetStack = nullptr;
};



UCLASS()
class ECLIPSE_API UEcpLayer : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	DECLARE_DELEGATE_TwoParams(FOnCompleteDisplayedWidget, UCommonActivatableWidget*, UEcpLayer*);
	FOnCompleteDisplayedWidget OnCompleteDisplayedWidget;

private:
	void OnDisplayedWidgetChanged(UCommonActivatableWidget* InWidget);
	void OnChangedTransitioning(UCommonActivatableWidgetContainerBase* InLayer, bool bIsTransitioning);

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
	virtual void NativeDestruct() override;
	virtual void NativeOnInitialized() override;

private:
	bool bTouchStart = false;


protected:
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FEcpLayerUIVariable UIVariable;
	
};
