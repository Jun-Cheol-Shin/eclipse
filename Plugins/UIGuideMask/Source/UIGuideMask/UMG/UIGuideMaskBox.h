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

	KeyEvent,
};

UENUM(BlueprintType)
enum class EActionCompletionPolicy : uint8
{
	Immediate,			  // Layer에서 터치/클릭 인식 즉시 완료 (팁/다음으로)
	OnTargetHandled,      // 타겟 위젯의 ‘성공 시그널’을 확인한 뒤 완료 (버튼 클릭 등)
	OnPredicate			  // 임의 조건(프레딕트) 충족 시 완료 (확장됨, 선택됨, 텍스트 유효 등)
};

USTRUCT(BlueprintType)
struct FGuideBoxActionParameters
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EGuideActionType ActionType = EGuideActionType::Click;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "EGuideActionType::Click != ActionType && EGuideActionType::KeyEvent != ActionType", EditConditionHides))
	float DragThreshold = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "EGuideActionType::KeyEvent == ActionType", EditConditionHides))
	FKey ActivationKey = FKey();
};

UCLASS()
class UIGUIDEMASK_API UUIGuideMaskBox : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	DECLARE_DELEGATE(FOnPostActionSignature)
	FOnPostActionSignature OnPostAction;

private:
	UPROPERTY(EditDefaultsOnly, Transient, meta = (AllowPrivateAccess = "true"))
	EGuideActionType ActionType = EGuideActionType::Click;

	UPROPERTY(EditDefaultsOnly, Transient, meta = (AllowPrivateAccess = "true", ClampMin = "10", ClampMax = "100"))
	float DragThreshold = 100.f;

	float ActionDPIScale = 0.f;
	float CorrectedDragThreshold = 0.f;
	
public:
	void ForceComplete();
	void SetBox(UWidget* InWidget, const FGuideBoxActionParameters& InParams);
	
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

private:
	FKey ActivationKey = FKey();
	FVector2D TouchStartPos = FVector2D();
	TWeakObjectPtr<UWidget> HighlightWidget = nullptr;
	EButtonClickMethod::Type CachedClickMethod = EButtonClickMethod::DownAndUp;
	EButtonTouchMethod::Type CachedTouchMethod = EButtonTouchMethod::DownAndUp;
};