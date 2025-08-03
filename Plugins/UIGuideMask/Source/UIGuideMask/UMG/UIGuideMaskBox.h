// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "UIGuideMaskBox.generated.h"


UENUM(BlueprintType)
enum class EGuideActionType : uint8
{
	Click,
	Drag,
	Swipe_Up,
	Swipe_Down,
	Swipe_Left,
	Swipe_Right,
};


UCLASS()
class UIGUIDEMASK_API UUIGuideMaskBox : public UCommonUserWidget
{
	GENERATED_BODY()
	
public:
	void SetBox(EGuideActionType InActionType, UWidget* InWidget);
	
private:
	void OnStartedClick(const FGeometry& InGeometry, const FPointerEvent& InEvent);
	void OnMoved(const FGeometry& InGeometry, const FPointerEvent& InEvent);


	void OnEndedSwipe(const FGeometry& InGeometry, const FPointerEvent& InEvent);
	void OnEndedDrag(const FGeometry& InGeometry, const FPointerEvent& InEvent);
	void OnEndedClick(const FGeometry& InGeometry, const FPointerEvent& InEvent);

protected:
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	//virtual FReply NativeOnTouchStarted(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent) override;
	//virtual FReply NativeOnTouchMoved(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent) override;
	//virtual FReply NativeOnTouchEnded(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent) override;

private:
	EGuideActionType ActionType = EGuideActionType::Click;
	TWeakObjectPtr<UWidget> HighlightWidget = nullptr;
	EButtonClickMethod::Type CachedClickMethod = EButtonClickMethod::DownAndUp;
	EButtonTouchMethod::Type CachedTouchMethod = EButtonTouchMethod::DownAndUp;
};
