// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "UIGuideMaskBox.generated.h"


UENUM(BlueprintType)
enum class EGuideActionType : uint8
{
	DownAndUp,
	Hold,

	Drag,
	Swipe_Up,
	Swipe_Down,
	Swipe_Left,
	Swipe_Right,
};


USTRUCT(BlueprintType)
struct FGuideBoxActionParameters
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EGuideActionType ActionType = EGuideActionType::DownAndUp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "EGuideActionType::DownAndUp == ActionType || EGuideActionType::Hold == ActionType", EditConditionHides))
	FKey ActivationKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "EGuideActionType::DownAndUp != ActionType && EGuideActionType::KeyEvent != ActionType && EGuideActionType::Hold != ActionType", EditConditionHides))
	float DragThresholdVectorSize = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "EGuideActionType::Hold == ActionType", EditConditionHides))
	float HoldSeconds = 0.f;
};

class UProgressBar;


UCLASS()
class UIGUIDEMASK_API UUIGuideMaskBox : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	DECLARE_DELEGATE(FOnPostActionSignature)
	FOnPostActionSignature OnPostAction;

public:
	void ForceComplete();
	void SetBox(UWidget* InWidget);
	void SetBoxAction(const FGuideBoxActionParameters& InActionParam);
	void Clear();
	
private:
	void OnResizedViewport(FViewport* InViewport, uint32 InWindowMode /*?*/);
	void OnChangedVisibility(ESlateVisibility InVisiblity);

	FReply OnStartedClick(const FGeometry& InGeometry, const FPointerEvent& InEvent);
	FReply OnMoved(const FGeometry& InGeometry, const FPointerEvent& InEvent);
	FReply OnEndedClick(const FGeometry& InGeometry, const FPointerEvent& InEvent);
	void OnEndedAction(const FPointerEvent& InEvent = FPointerEvent());

	FReply OnStartedKeyEvent(const FGeometry& InGeometry, const FKeyEvent& InEvent);
	FReply OnEndedKeyEvent(const FGeometry& InGeometry, const FKeyEvent& InEvent);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& InGeometry, float InDeltaTime) override;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual FReply NativeOnTouchStarted(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent) override;
	virtual FReply NativeOnTouchMoved(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent) override;
	virtual FReply NativeOnTouchEnded(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent) override;

	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;


	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	
private:
	bool IsCorrectSwipe(const FVector2D& InMoveVec);
	FPointerEvent CreateMouseLikePointerEventFromTouch(const FPointerEvent& InTouchEvent);
	bool IsDragType(EGuideActionType InType) const;

private:
	FVector2D TouchStartPos = FVector2D();
	float DragThreshold = 100.f;
	float ActionDPIScale = 0.f;
	float CorrectedDragThreshold = 0.f;
	double StartTime = 0.f;
	double HoldSeconds = 0.f;

	EButtonClickMethod::Type CachedClickMethod = EButtonClickMethod::DownAndUp;
	EButtonTouchMethod::Type CachedTouchMethod = EButtonTouchMethod::DownAndUp;


private:
	FKey ActivationKey {};
	EGuideActionType ActionType {};

	TWeakObjectPtr<UWidget> HighlightWidget = nullptr;

private:
	UPROPERTY(meta = (BindWidget, AllowPrivateAccess = "true"))
	UProgressBar* HoldProgressBar = nullptr;
};