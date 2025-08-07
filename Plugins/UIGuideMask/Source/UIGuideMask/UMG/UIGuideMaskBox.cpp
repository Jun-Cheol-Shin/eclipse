// Fill out your copyright notice in the Description page of Project Settings.


#include "UIGuideMaskBox.h"

#include "Components/Button.h"
#include "Components/CheckBox.h"

#include "Blueprint/WidgetLayoutLibrary.h"



void UUIGuideMaskBox::SetBox(UWidget* InWidget, const FGuideBoxActionParameters& InParams)
{
	HighlightWidget = InWidget;

	ActionType = InParams.ActionType;
	DragThreshold = InParams.DragThreshold;
	ActionDPIScale = UWidgetLayoutLibrary::GetViewportScale(this);

	CorrectedDragThreshold = DragThreshold * ActionDPIScale;
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

void UUIGuideMaskBox::OnStartedClick(const FGeometry& InGeometry, const FPointerEvent& InEvent)
{
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
				SlateWidget->OnMouseButtonDown(InGeometry, InEvent);
			}
		}
	}
}

void UUIGuideMaskBox::OnMoved(const FGeometry& InGeometry, const FPointerEvent& InEvent)
{
	//if (false == HighlightWidget.IsValid()) return;

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
			}
		}

		if (CorrectedDragThreshold <= MoveVec.Size())
		{
			UE_LOG(LogTemp, Warning, TEXT("Complete Drag!"));
			OnEndedClick(InGeometry, InEvent);
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
				}
			}

			if (CorrectedDragThreshold <= MoveVec.Size())
			{
				UE_LOG(LogTemp, Warning, TEXT("Complete Drag!"));
				OnEndedClick(InGeometry, InEvent);
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

}

void UUIGuideMaskBox::OnEndedClick(const FGeometry& InGeometry, const FPointerEvent& InEvent)
{
	OnPreAction.ExecuteIfBound();
	UE_LOG(LogTemp, Warning, TEXT("Pre Action"));

	if (HighlightWidget.IsValid())
	{
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
	}

	OnEndedAction(InGeometry, InEvent);
}

void UUIGuideMaskBox::OnEndedAction(const FGeometry& InGeometry, const FPointerEvent& InEvent)
{
	UE_LOG(LogTemp, Warning, TEXT("Action End"));
	OnPostAction.ExecuteIfBound();

	TouchStartPos = FVector2D::Zero();
	HighlightWidget.Reset();
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
	OnStartedClick(InGeometry, InMouseEvent);

	return FReply::Handled();
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
	TouchStartPos = FVector2D::Zero();

	switch (ActionType)
	{
	case EGuideActionType::Click: 
	{
		OnEndedClick(InGeometry, InMouseEvent);
	}
		break;
	default:
		break;
	}


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

void UUIGuideMaskBox::NativeConstruct()
{
	Super::NativeConstruct();

	SetConsumePointerInput(true);

	OnNativeVisibilityChanged.RemoveAll(this);
	OnNativeVisibilityChanged.AddUObject(this, &UUIGuideMaskBox::OnChangedVisibility);

	//FViewport::ViewportResizedEvent.AddUObject(this, &U::OnViewportResized);
	//FSlateApplication::Get().GetGameViewport();
}

void UUIGuideMaskBox::NativeDestruct()
{
	Super::NativeDestruct();

	OnNativeVisibilityChanged.RemoveAll(this);
}
