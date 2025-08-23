// Fill out your copyright notice in the Description page of Project Settings.


#include "UIGuideMaskBox.h"

#include "Components/Button.h"
#include "Components/CheckBox.h"
#include "Components/ProgressBar.h"

#include "Blueprint/WidgetLayoutLibrary.h"


void UUIGuideMaskBox::Clear()
{
	StartTime = 0.f;
	TouchStartPos = FVector2D::Zero();
	HighlightWidget.Reset();
}

void UUIGuideMaskBox::ForceComplete()
{
	Clear();

	OnPostAction.ExecuteIfBound();
}

void UUIGuideMaskBox::SetBox(UWidget* InWidget)
{
	HighlightWidget = InWidget;
}

void UUIGuideMaskBox::SetBoxAction(const FGuideBoxActionParameters& InActionParam)
{
	if (nullptr != HoldProgressBar && nullptr != HoldProgressBar->GetParent())
	{
		HoldProgressBar->GetParent()->SetVisibility(ESlateVisibility::Collapsed);
		HoldProgressBar->SetPercent(0.f);
	}

	ActionType = InActionParam.ActionType;
	ActivationKey = InActionParam.ActivationKey;

	if (true == IsDragType(InActionParam.ActionType))
	{
		DragThreshold = InActionParam.DragThresholdVectorSize;
		ActionDPIScale = UWidgetLayoutLibrary::GetViewportScale(this);

		CorrectedDragThreshold = DragThreshold * ActionDPIScale;
	}

	else if (EGuideActionType::Hold == InActionParam.ActionType)
	{
		HoldSeconds = InActionParam.HoldSeconds;
	}
}


void UUIGuideMaskBox::OnResizedViewport(FViewport* InViewport, uint32 InWindowMode /*?*/)
{
	if (nullptr == InViewport || nullptr == InViewport->GetClient()) return;

	ActionDPIScale = InViewport->GetClient()->GetDPIScale();
	CorrectedDragThreshold = DragThreshold * ActionDPIScale;
}

void UUIGuideMaskBox::OnChangedVisibility(ESlateVisibility InVisiblity)
{
	switch (InVisiblity)
	{
	case ESlateVisibility::Visible:	
	case ESlateVisibility::HitTestInvisible:
	case ESlateVisibility::SelfHitTestInvisible:
	{
		FViewport::ViewportResizedEvent.AddUObject(this, &UUIGuideMaskBox::OnResizedViewport);
	}
		break;
	case ESlateVisibility::Collapsed:
	case ESlateVisibility::Hidden:
	{
		FViewport::ViewportResizedEvent.RemoveAll(this);
	}
		break;

	default:
		break;
	}
}
FReply UUIGuideMaskBox::OnStartedClick(const FGeometry& InGeometry, const FPointerEvent& InEvent)
{
	if (false == InGeometry.IsUnderLocation(InEvent.GetScreenSpacePosition())) return FReply::Unhandled();

	TouchStartPos = InGeometry.AbsoluteToLocal(InEvent.GetScreenSpacePosition());

	if (HighlightWidget.IsValid())
	{
		if (UButton* ButtonWidget = Cast<UButton>(HighlightWidget))
		{
			CachedClickMethod = ButtonWidget->GetClickMethod();
			CachedTouchMethod = ButtonWidget->GetTouchMethod();

			ButtonWidget->SetClickMethod(EButtonClickMethod::PreciseClick);
			ButtonWidget->SetTouchMethod(EButtonTouchMethod::PreciseTap);

			if (HighlightWidget.IsValid())
			{
				TSharedRef<SWidget> ButtonSlateWidget = ButtonWidget->TakeWidget();
				if (ensure(&ButtonSlateWidget))
				{
					ButtonSlateWidget.Get().OnMouseButtonDown(InGeometry, InEvent);
				}
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
				if (InEvent.IsTouchEvent())
				{
					SlateWidget->OnTouchStarted(InGeometry, InEvent);
				}

				else
				{
					SlateWidget->OnMouseButtonDown(InGeometry, InEvent);
				}
			}
		}

		return FReply::Handled().CaptureMouse(TakeWidget());
	}

	return FReply::Unhandled();
}
FReply UUIGuideMaskBox::OnMoved(const FGeometry& InGeometry, const FPointerEvent& InEvent)
{
	if (TouchStartPos.IsZero()) return FReply::Unhandled();

	float DPIScale = UWidgetLayoutLibrary::GetViewportScale(this);

	FVector2D CurrentPosition = InGeometry.AbsoluteToLocal(InEvent.GetScreenSpacePosition());
	FVector2D MoveVec = CurrentPosition - TouchStartPos;

	switch (ActionType)
	{
	case EGuideActionType::Drag:
	{
		if (HighlightWidget.IsValid())
		{
			TSharedRef<SWidget> SlateWidget = HighlightWidget->TakeWidget();
			if (ensure(&SlateWidget))
			{
				SlateWidget->OnMouseMove(InGeometry, InEvent);
				SlateWidget->OnTouchMoved(InGeometry, InEvent);
			}
		}

		if (CorrectedDragThreshold <= MoveVec.Size())
		{
			UE_LOG(LogTemp, Warning, TEXT("Complete Drag!"));
			OnEndedAction(InEvent);
		}
	}
		break;
	case EGuideActionType::Swipe_Up:
	case EGuideActionType::Swipe_Down:
	case EGuideActionType::Swipe_Left:
	case EGuideActionType::Swipe_Right:
	{
		if (true == IsCorrectSwipe(MoveVec))
		{
			UE_LOG(LogTemp, Warning, TEXT("Success Swipe!"));

			if (HighlightWidget.IsValid())
			{
				TSharedRef<SWidget> SlateWidget = HighlightWidget->TakeWidget();
				if (ensure(&SlateWidget))
				{
					SlateWidget->OnMouseMove(InGeometry, InEvent);
					SlateWidget->OnTouchMoved(InGeometry, InEvent);
				}
			}

			if (CorrectedDragThreshold <= MoveVec.Size())
			{
				UE_LOG(LogTemp, Warning, TEXT("Complete Drag!"));
				OnEndedAction(InEvent);
			}
		}

		else
		{
			TouchStartPos = FVector2D::Zero();
		}
	}
		break;
	default:
		break;
	}

	return FReply::Handled();
}
FReply UUIGuideMaskBox::OnEndedClick(const FGeometry& InGeometry, const FPointerEvent& InEvent)
{
	UE_LOG(LogTemp, Warning, TEXT("Pre Action"));

	//FVector2D CurrentPosition = InGeometry.AbsoluteToLocal(InEvent.GetScreenSpacePosition());
	if (false == InGeometry.IsUnderLocation(InEvent.GetScreenSpacePosition())) return FReply::Unhandled();

	if (HighlightWidget.IsValid())
	{
		if (UButton* ButtonWidget = Cast<UButton>(HighlightWidget))
		{
			ButtonWidget->SetClickMethod(EButtonClickMethod::MouseUp);
			ButtonWidget->SetTouchMethod(EButtonTouchMethod::PreciseTap);

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
			CheckBoxWidget->SetClickMethod(EButtonClickMethod::MouseUp);
			CheckBoxWidget->SetTouchMethod(EButtonTouchMethod::PreciseTap);

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
				if (InEvent.IsTouchEvent())
				{
					SlateWidget->OnTouchEnded(InGeometry, InEvent);
				}
				
				else
				{
					SlateWidget->OnMouseButtonUp(InGeometry, InEvent);
				}
			}
		}

		OnEndedAction(InEvent);
		return FReply::Handled().ReleaseMouseCapture();
	}

	return FReply::Unhandled();
}

FReply UUIGuideMaskBox::OnStartedKeyEvent(const FGeometry& InGeometry, const FKeyEvent& InEvent)
{
	TSharedRef<SWidget> SlateWidget = HighlightWidget->TakeWidget();
	if (ensure(&SlateWidget))
	{
		SlateWidget->OnKeyDown(InGeometry, InEvent);
	}

	return FReply::Handled();
}

FReply UUIGuideMaskBox::OnEndedKeyEvent(const FGeometry& InGeometry, const FKeyEvent& InEvent)
{
	TSharedRef<SWidget> SlateWidget = HighlightWidget->TakeWidget();
	if (ensure(&SlateWidget))
	{
		SlateWidget->OnKeyUp(InGeometry, InEvent);
	}

	OnEndedAction();

	return FReply::Handled();
}

void UUIGuideMaskBox::OnEndedAction(const FPointerEvent& InEvent)
{
	UE_LOG(LogTemp, Warning, TEXT("Action End"));

	if (false == InEvent.GetPressedButtons().IsEmpty())
	{
		NativeOnMouseLeave(InEvent);
	}


	Clear();

	OnPostAction.ExecuteIfBound();

}

bool UUIGuideMaskBox::IsCorrectSwipe(const FVector2D& InMoveVec)
{
	float XValue = FMath::Abs(InMoveVec.X);
	float YValue = FMath::Abs(InMoveVec.Y);


	switch (ActionType)
	{
	case EGuideActionType::Swipe_Up:
	{
		if (XValue <= YValue && InMoveVec.Y < 0)
		{
			return true;
		}
	}
		break;
	case EGuideActionType::Swipe_Down:
	{
		if (XValue <= YValue && InMoveVec.Y > 0)
		{
			return true;
		}
	}
		break;
	case EGuideActionType::Swipe_Left:
	{
		if (XValue >= YValue && InMoveVec.X < 0)
		{
			return true;
		}
	}
		break;
	case EGuideActionType::Swipe_Right:
	{
		if (XValue >= YValue && InMoveVec.X > 0)
		{
			return true;
		}
	}
		break;
	default:
		break;
	}




	return false;
}


FReply UUIGuideMaskBox::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() != ActivationKey)
	{
		return FReply::Unhandled();
	}

	else if (EGuideActionType::Hold == ActionType)
	{
		StartTime = FPlatformTime::Seconds();
		TouchStartPos = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
		
		if (nullptr != HoldProgressBar && HoldProgressBar->GetParent())
		{
			HoldProgressBar->GetParent()->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}

		return FReply::Handled();
	}

	return OnStartedClick(InGeometry, InMouseEvent);
}

FReply UUIGuideMaskBox::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (true == TouchStartPos.IsZero())
	{
		return FReply::Unhandled();
	}

	switch (ActionType)
	{
	case EGuideActionType::Drag:
	case EGuideActionType::Swipe_Up:
	case EGuideActionType::Swipe_Down:
	case EGuideActionType::Swipe_Left:
	case EGuideActionType::Swipe_Right:
	{
		OnMoved(InGeometry, InMouseEvent);
	}
		break;
	default:
		break;
	}

	return FReply::Handled();
}

FReply UUIGuideMaskBox::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (nullptr != HoldProgressBar && HoldProgressBar->GetParent())
	{
		HoldProgressBar->GetParent()->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (false == TouchStartPos.IsZero())
	{
		TouchStartPos = FVector2D::Zero();
		StartTime = 0.f;

		switch (ActionType)
		{
		case EGuideActionType::DownAndUp:
		{
			if (InMouseEvent.GetEffectingButton() == ActivationKey)
			{
				return OnEndedClick(InGeometry, InMouseEvent);
			}
		}
		break;
		default:
			break;
		}
	}

	return FReply::Unhandled();
}

FReply UUIGuideMaskBox::NativeOnTouchStarted(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)
{
	if (EGuideActionType::Hold == ActionType)
	{
		StartTime = FPlatformTime::Seconds();
		TouchStartPos = InGeometry.AbsoluteToLocal(InGestureEvent.GetScreenSpacePosition());

		if (nullptr != HoldProgressBar && HoldProgressBar->GetParent())
		{
			HoldProgressBar->GetParent()->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}

		return FReply::Handled();
	}

	return OnStartedClick(InGeometry, InGestureEvent);
}

FReply UUIGuideMaskBox::NativeOnTouchMoved(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)
{
	if (true == TouchStartPos.IsZero())
	{
		return FReply::Unhandled();
	}

	switch (ActionType)
	{
	case EGuideActionType::Drag:
	case EGuideActionType::Swipe_Up:
	case EGuideActionType::Swipe_Down:
	case EGuideActionType::Swipe_Left:
	case EGuideActionType::Swipe_Right:
	{
		OnMoved(InGeometry, InGestureEvent);
	}
	break;
	default:
		break;
	}

	return FReply::Handled();
}

FReply UUIGuideMaskBox::NativeOnTouchEnded(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)
{
	if (nullptr != HoldProgressBar && HoldProgressBar->GetParent())
	{
		HoldProgressBar->GetParent()->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (false == TouchStartPos.IsZero())
	{
		TouchStartPos = FVector2D::Zero();
		StartTime = 0.f;

		switch (ActionType)
		{
		case EGuideActionType::DownAndUp:
		{
			return OnEndedClick(InGeometry, InGestureEvent);
		}
		break;
		default:
			break;
		}
	}

	return FReply::Unhandled();
}

FReply UUIGuideMaskBox::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == ActivationKey)
	{
		if (EGuideActionType::Hold == ActionType && true == TouchStartPos.IsZero())
		{
			StartTime = FPlatformTime::Seconds();
			TouchStartPos = InGeometry.AbsoluteToLocal(FSlateApplication::Get().GetCursorPos());

			if (nullptr != HoldProgressBar && HoldProgressBar->GetParent())
			{
				HoldProgressBar->GetParent()->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			}

			return OnStartedKeyEvent(InGeometry, InKeyEvent);
		}

		else
		{
			return OnStartedKeyEvent(InGeometry, InKeyEvent);
		}
	}

	return FReply::Unhandled();
}

FReply UUIGuideMaskBox::NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (nullptr != HoldProgressBar && HoldProgressBar->GetParent())
	{
		HoldProgressBar->GetParent()->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (EGuideActionType::Hold == ActionType && false == TouchStartPos.IsZero())
	{
		StartTime = 0.f;
		TouchStartPos = FVector2D::Zero();

		return FReply::Handled();
	}

	else if (InKeyEvent.GetKey() == ActivationKey)
	{
		return OnEndedKeyEvent(InGeometry, InKeyEvent);
	}

	return FReply::Unhandled();
}

void UUIGuideMaskBox::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (HighlightWidget.IsValid())
	{
		TSharedRef<SWidget> SlateWidget = HighlightWidget->TakeWidget();
		if (ensure(&SlateWidget))
		{
			SlateWidget.Get().OnMouseEnter(InGeometry, InMouseEvent);
		}
	}
}

void UUIGuideMaskBox::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	if (nullptr != HoldProgressBar && HoldProgressBar->GetParent())
	{
		HoldProgressBar->GetParent()->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (HighlightWidget.IsValid())
	{
		TSharedRef<SWidget> SlateWidget = HighlightWidget->TakeWidget();
		if (ensure(&SlateWidget))
		{
			SlateWidget.Get().OnMouseLeave(InMouseEvent);
		}
		

		if (UButton* ButtonWidget = Cast<UButton>(HighlightWidget))
		{
			ButtonWidget->SetClickMethod(CachedClickMethod);
			ButtonWidget->SetTouchMethod(CachedTouchMethod);
		}

		else if (UCheckBox* CheckBoxWidget = Cast<UCheckBox>(HighlightWidget))
		{
			CheckBoxWidget->SetClickMethod(CachedClickMethod);
			CheckBoxWidget->SetTouchMethod(CachedTouchMethod);
		}
	}

	if (false == IsDragType(ActionType))
	{
		TouchStartPos = FVector2D::Zero();
	}

	if (EGuideActionType::Hold != ActionType)
	{
		StartTime = 0.f;
	}
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

bool UUIGuideMaskBox::IsDragType(EGuideActionType InType) const
{
	return  InType == EGuideActionType::Drag ||
			InType == EGuideActionType::Swipe_Up ||
			InType == EGuideActionType::Swipe_Down ||
			InType == EGuideActionType::Swipe_Left ||
			InType == EGuideActionType::Swipe_Right;
}

void UUIGuideMaskBox::NativeConstruct()
{
	Super::NativeConstruct();

	SetConsumePointerInput(true);

	OnNativeVisibilityChanged.RemoveAll(this);
	OnNativeVisibilityChanged.AddUObject(this, &UUIGuideMaskBox::OnChangedVisibility);

	SetIsFocusable(true);
}

void UUIGuideMaskBox::NativeDestruct()
{
	Super::NativeDestruct();

	OnNativeVisibilityChanged.RemoveAll(this);
}

void UUIGuideMaskBox::NativeTick(const FGeometry& InGeometry, float InDeltaTime)
{
	Super::NativeTick(InGeometry, InDeltaTime);

	if (false == TouchStartPos.IsZero() && 
		ActionType == EGuideActionType::Hold)
	{
		if (FPlatformTime::Seconds() > StartTime + HoldSeconds)
		{
			OnEndedAction();
		}

		else if (nullptr != HoldProgressBar)
		{
			double Value = (FPlatformTime::Seconds() - StartTime) / HoldSeconds;

			HoldProgressBar->SetPercent(Value);
		}

	}
}
