// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "NLGameLayer.generated.h"


class UCommonActivatableWidget;
class UCommonActivatableWidgetStack;
class UCommonActivatableWidgetQueue;
class UCommonActivatableWidgetContainerBase;


UENUM(BlueprintType)
enum class ELayerInputMode : uint8
{
	GameOnly,
	GameAndUI,
	UIOnly,
};

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


UCLASS(MinimalAPI)
class UNLGameLayer : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnCompleteDisplayedWidget, UCommonActivatableWidget*, UNLGameLayer*, bool /* bIsShow */);
	FOnCompleteDisplayedWidget OnCompleteDisplayedWidgetDelegate;

	UCommonActivatableWidgetContainerBase* GetActivatableWidgetContainer() const;
	int GetActivatedWidgetNum() const;
	bool IsOnlyOneWidget(UCommonActivatableWidget* InWidget);

	const TArray<UCommonActivatableWidget*>& GetWidgetList() const;


protected:
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FEcpLayerUIVariable UIVariable;


	// callback
private:
	void OnDisplayedWidgetChanged(UCommonActivatableWidget* InWidget);
	//void OnChangedTransitioning(UCommonActivatableWidgetContainerBase* InLayer, bool bIsTransitioning);

protected:
	virtual void OnConstruct() {};
	virtual void OnDestruct() {};

	virtual void OnClick() {};
	virtual void OnTouch() {};

	
private:
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual FReply NativeOnTouchStarted(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent) override;
	virtual FReply NativeOnTouchMoved(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent) override;
	virtual FReply NativeOnTouchEnded(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent) override;
	
	virtual void NativeOnMouseLeave(const FPointerEvent& InEvent) override;

private:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeOnInitialized() override;

private:
	bool bTouchStart = false;
};
