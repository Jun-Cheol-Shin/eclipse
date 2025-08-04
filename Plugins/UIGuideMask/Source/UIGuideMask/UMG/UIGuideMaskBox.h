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


	DECLARE_DELEGATE(FOnPreActionSignature)
	FOnPreActionSignature OnPreAction;
	DECLARE_DELEGATE(FOnPostActionSignature)
	FOnPostActionSignature OnPostAction;

	
public:
	void SetBox(EGuideActionType InActionType, UWidget* InWidget);
	
private:
	void OnStartedClick(const FGeometry& InGeometry, const FPointerEvent& InEvent);
	void OnMoved(const FGeometry& InGeometry, const FPointerEvent& InEvent);
	void OnEndedClick(const FGeometry& InGeometry, const FPointerEvent& InEvent);

	void OnEndedAction(const FGeometry& InGeometry, const FPointerEvent& InEvent);

protected:
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	//virtual FReply NativeOnTouchStarted(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent) override;
	//virtual FReply NativeOnTouchMoved(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent) override;
	//virtual FReply NativeOnTouchEnded(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent) override;
	
private:
	bool IsCorrectSwipe(const FVector2D& InMoveVec);
	FPointerEvent CreateMouseLikePointerEventFromTouch(const FPointerEvent& InTouchEvent);

private:
	FVector2D TouchStartPos = FVector2D();

	EGuideActionType ActionType = EGuideActionType::Click;
	TWeakObjectPtr<UWidget> HighlightWidget = nullptr;
	EButtonClickMethod::Type CachedClickMethod = EButtonClickMethod::DownAndUp;
	EButtonTouchMethod::Type CachedTouchMethod = EButtonTouchMethod::DownAndUp;
};
