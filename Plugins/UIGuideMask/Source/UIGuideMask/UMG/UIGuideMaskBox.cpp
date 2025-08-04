// Fill out your copyright notice in the Description page of Project Settings.


#include "UIGuideMaskBox.h"
#include "Components/Button.h"
#include "Components/CheckBox.h"


void UUIGuideMaskBox::SetBox(EGuideActionType InActionType, UWidget* InWidget)
{
	ActionType = InActionType;
	HighlightWidget = InWidget;
}

void UUIGuideMaskBox::OnStartedClick(const FGeometry& InGeometry, const FPointerEvent& InEvent)
{
	TouchStartPos = InGeometry.AbsoluteToLocal(InEvent.GetScreenSpacePosition());

	if (UButton* ButtonWidget = Cast<UButton>(HighlightWidget))
	{
		CachedClickMethod = ButtonWidget->GetClickMethod();
		CachedTouchMethod = ButtonWidget->GetTouchMethod();

		ButtonWidget->SetClickMethod(EButtonClickMethod::PreciseClick);
		ButtonWidget->SetTouchMethod(EButtonTouchMethod::PreciseTap);

		TSharedRef<SWidget> ButtonSlateWidget = ButtonWidget->TakeWidget();
		if (ensure(&ButtonSlateWidget))
		{
			ButtonSlateWidget.Get().OnMouseButtonDown(InGeometry, InEvent);
		}
	}

	else if (UCheckBox* CheckBoxWidget = Cast<UCheckBox>(HighlightWidget))
	{
		CachedClickMethod = CheckBoxWidget->GetClickMethod();
		CachedTouchMethod = CheckBoxWidget->GetTouchMethod();

		CheckBoxWidget->SetClickMethod(EButtonClickMethod::PreciseClick);
		CheckBoxWidget->SetTouchMethod(EButtonTouchMethod::PreciseTap);

		TSharedRef<SWidget> CheckBoxSlateWidget = CheckBoxWidget->TakeWidget();
		if (ensure(&CheckBoxSlateWidget))
		{
			CheckBoxSlateWidget.Get().OnMouseButtonDown(InGeometry,
				CreateMouseLikePointerEventFromTouch(InEvent));
		}
	}

	else
	{
		TSharedRef<SWidget> SlateWidget = HighlightWidget->TakeWidget();
		if (ensure(&SlateWidget))
		{
			SlateWidget->OnMouseButtonDown(InGeometry, InEvent);
		}
	}
}

void UUIGuideMaskBox::OnMoved(const FGeometry& InGeometry, const FPointerEvent& InEvent)
{
	if (false == HighlightWidget.IsValid()) return;

	FVector2D CurrentPosition = InGeometry.AbsoluteToLocal(InEvent.GetScreenSpacePosition());

	switch (ActionType)
	{
	case EGuideActionType::Drag:
	{
		TSharedRef<SWidget> SlateWidget = HighlightWidget->TakeWidget();
		if (ensure(&SlateWidget))
		{
			SlateWidget->OnMouseMove(InGeometry, InEvent);
		}
	}
		break;
	case EGuideActionType::Swipe_Up:
	case EGuideActionType::Swipe_Down:
	case EGuideActionType::Swipe_Left:
	case EGuideActionType::Swipe_Right:
	{

		if (true == IsCorrectSwipe(CurrentPosition - TouchStartPos))
		{
			TSharedRef<SWidget> SlateWidget = HighlightWidget->TakeWidget();
			if (ensure(&SlateWidget))
			{
				SlateWidget->OnMouseMove(InGeometry, InEvent);
			}
		}
	}
		break;
	default:
		break;
	}


	// TODO: 드래그를 정해줄 기준값 필요
	float DragValue = FMath::Abs(FVector2D::Distance(CurrentPosition, TouchStartPos));

	if (10.f <= DragValue)
	{
		OnEndedAction(InGeometry, InEvent);
	}
}

void UUIGuideMaskBox::OnEndedClick(const FGeometry& InGeometry, const FPointerEvent& InEvent)
{
	OnPreAction.ExecuteIfBound();

	if (UButton* ButtonWidget = Cast<UButton>(HighlightWidget))
	{
		TSharedRef<SWidget> ButtonSlateWidget = ButtonWidget->TakeWidget();
		if (ensure(&ButtonSlateWidget))
		{
			ButtonSlateWidget.Get().OnMouseButtonUp(InGeometry, InEvent);
		}

		ButtonWidget->SetClickMethod(CachedClickMethod);
		ButtonWidget->SetTouchMethod(CachedTouchMethod);
	}

	else if (UCheckBox* CheckBoxWidget = Cast<UCheckBox>(HighlightWidget))
	{
		TSharedRef<SWidget> CheckBoxSlateWidget = CheckBoxWidget->TakeWidget();
		if (ensure(&CheckBoxSlateWidget))
		{
			CheckBoxSlateWidget.Get().OnMouseButtonUp(InGeometry,
				CreateMouseLikePointerEventFromTouch(InEvent));
		}


		CheckBoxWidget->SetClickMethod(CachedClickMethod);
		CheckBoxWidget->SetTouchMethod(CachedTouchMethod);

	}

	else
	{
		TSharedRef<SWidget> SlateWidget = HighlightWidget->TakeWidget();
		if (ensure(&SlateWidget))
		{
			SlateWidget->OnMouseButtonUp(InGeometry, InEvent);
		}
	}

	OnEndedAction(InGeometry, InEvent);
}

void UUIGuideMaskBox::OnEndedAction(const FGeometry& InGeometry, const FPointerEvent& InEvent)
{
	OnPostAction.ExecuteIfBound();

	HighlightWidget.Reset();
	TouchStartPos = FVector2D::Zero();
}

bool UUIGuideMaskBox::IsCorrectSwipe(const FVector2D& InMoveVec)
{

	return false;
}


FReply UUIGuideMaskBox::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	OnStartedClick(InGeometry, InMouseEvent);

	return FReply::Handled();
}

FReply UUIGuideMaskBox::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	OnMoved(InGeometry, InMouseEvent);

	return FReply::Handled();
}

FReply UUIGuideMaskBox::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	OnEndedClick(InGeometry, InMouseEvent);

	return FReply::Handled();
}

FPointerEvent UUIGuideMaskBox::CreateMouseLikePointerEventFromTouch(const FPointerEvent& InTouchEvent)
{
	return FPointerEvent(
		InTouchEvent.GetPointerIndex(),
		InTouchEvent.GetScreenSpacePosition(),
		InTouchEvent.GetLastScreenSpacePosition(),
		{},
		EKeys::LeftMouseButton,
		InTouchEvent.GetWheelDelta(),
		InTouchEvent.GetModifierKeys()
	);
}
