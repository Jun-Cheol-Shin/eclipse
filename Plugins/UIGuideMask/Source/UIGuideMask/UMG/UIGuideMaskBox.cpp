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

	else
	{
		TSharedRef<SWidget> SlateWidget = ButtonWidget->TakeWidget();
		if (ensure(&SlateWidget))
		{
			SlateWidget->OnMouseButtonDown(InGeometry, InEvent);
		}
	}
}

void UUIGuideMaskBox::OnMoved(const FGeometry& InGeometry, const FPointerEvent& InEvent)
{

}

void UUIGuideMaskBox::OnEndedSwipe(const FGeometry& InGeometry, const FPointerEvent& InEvent)
{

}

void UUIGuideMaskBox::OnEndedDrag(const FGeometry& InGeometry, const FPointerEvent& InEvent)
{

}

void UUIGuideMaskBox::OnEndedClick(const FGeometry& InGeometry, const FPointerEvent& InEvent)
{



	HighlightWidget.Reset();
}

FReply UUIGuideMaskBox::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	OnStartedClick(InGeometry, InMouseEvent);

	return FReply::Handled();
}

FReply UUIGuideMaskBox::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{




	return FReply::Handled();
}

FReply UUIGuideMaskBox::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	OnEndedClick(InGeometry, InMouseEvent);

	return FReply::Handled();
}
