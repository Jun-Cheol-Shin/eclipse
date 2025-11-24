// Fill out your copyright notice in the Description page of Project Settings.


#include "Draggable.h"
#include "DragDetectable.h"
#include "DragPayload.h"

#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/Widget.h"

#include "Blueprint/SlateBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Slate/UMGDragDropOp.h"

// Add default functionality here for any IDraggable functions that are not pure virtual.

void IDraggable::SetWorld(UWorld* InWorld)
{
	OuterWorld = InWorld;
}

void IDraggable::SetOwningController(APlayerController* InController)
{
	if (nullptr == InController)
	{
		return;
	}

	OuterWorld = InController->GetWorld();
}

void IDraggable::Reset()
{
	OuterWorld.Reset();

	if (EventWidget.IsValid())
	{
		TSharedPtr<SWidget> SlateWidget = EventWidget->TakeWidget();
		if (SlateWidget.IsValid())
		{
			SlateWidget->SetOnMouseButtonDown(FPointerEventHandler());
			SlateWidget->SetOnMouseDoubleClick(FPointerEventHandler());
			SlateWidget->SetOnMouseMove(FPointerEventHandler());
			SlateWidget->SetOnMouseButtonUp(FPointerEventHandler());
			SlateWidget->SetOnMouseEnter(FNoReplyPointerEventHandler());
			SlateWidget->SetOnMouseLeave(FSimpleNoReplyPointerEventHandler());
		}
	}

	EventWidget.Reset();
}

void IDraggable::SetEventFromImage(UImage* InImage)
{
	SetEvent(InImage);
}

void IDraggable::SetEventFromBorder(UBorder* InBorder)
{
	SetEvent(InBorder);
}

void IDraggable::SetEnableToggle(bool bIsEnable)
{
	//bUseToogle = bIsEnable;
}

UDragPayload* IDraggable::CreateDragPayload(TFunction<void(UDragPayload*)> InFunc)
{
	UDragDropOperation* Operation = UWidgetBlueprintLibrary::CreateDragDropOperation(UDragPayload::StaticClass());

	if (UDragPayload* Payload = Cast<UDragPayload>(Operation))
	{
		Payload->Set(Cast<UUserWidget>(this));
		InFunc(Payload);

		return Payload;
	}

	return nullptr;
}

FReply IDraggable::MouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InEvent)
{
	if (InEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		UUserWidget* OuterWidget = Cast<UUserWidget>(this);
		if (OuterWidget)
		{
			OuterPanelWidget = OuterWidget->GetParent();
		}

		UWidgetBlueprintLibrary::GetAllWidgetsWithInterface(OuterWorld.Get(), OUT DetectableWidgets, UDragDetectable::StaticClass(), false);

		return UWidgetBlueprintLibrary::DetectDragIfPressed(InEvent, Cast<UWidget>(this), EKeys::LeftMouseButton).NativeReply;
	}


	return FReply::Unhandled();
}

void IDraggable::Drag(UUserWidget* InVisualWidget, EDragPivot InPivot, const FVector2D& InOffset, const FPointerEvent& InEvent)
{
	if (nullptr == InVisualWidget)
	{
		return;
	}

	FVector2D Cursor_PixelPosition;
	FVector2D Cursor_ViewportPosition;
	USlateBlueprintLibrary::AbsoluteToViewport(OuterWorld.Get(), InEvent.GetScreenSpacePosition(), OUT Cursor_PixelPosition, OUT Cursor_ViewportPosition);

	FVector2D Widget_PixelPosition;
	FVector2D Widget_ViewportPosition;
	USlateBlueprintLibrary::AbsoluteToViewport(OuterWorld.Get(), InVisualWidget->GetTickSpaceGeometry().GetAbsolutePosition(), OUT Widget_PixelPosition, OUT Widget_ViewportPosition);

	const float DPIScale = UWidgetLayoutLibrary::GetViewportScale(OuterWorld->GetGameViewport());
	const FVector2D WidgetSize = InVisualWidget->GetDesiredSize() * DPIScale;
	const FVector2D ViewportSize = UWidgetLayoutLibrary::GetViewportSize(OuterWorld.Get());

	const FVector2D ClickOffset = GetClickOffset(InPivot, InOffset);

	/*
	
	FVector2D FinalPos;
		FinalPos.X = FMath::Clamp(WidgetPos.X, 0.0f, ViewportSize.X - WidgetSize.X);
		FinalPos.Y = FMath::Clamp(WidgetPos.Y, 0.0f, ViewportSize.Y - WidgetSize.Y);

		OuterWidget->SetPositionInViewport(FinalPos);

		NativeOnDrag(InGeometry, InEvent);

		if (nullptr == CurrentDetectedWidget)
		{
			for (auto& Widget : DetectableWidgets)
			{
				if (nullptr == Widget) continue;

				else if (true == Widget->GetTickSpaceGeometry().IsUnderLocation(InEvent.GetScreenSpacePosition()))
				{
					CurrentDetectedWidget = Widget;
					break;
				}
			}

			if (IDragDetectable* DetectableInterface = Cast<IDragDetectable>(CurrentDetectedWidget))
			{
				DetectableInterface->NativeOnStartDetectDrag(OuterWidget, InEvent);
			}

			else if (CurrentDetectedWidget->StaticClass() && CurrentDetectedWidget->StaticClass()->ImplementsInterface(UDragDetectable::StaticClass()))
			{
				IDragDetectable::Execute_OnStartDetectDrag(CurrentDetectedWidget, OuterWidget, InEvent);
			}
		}

		else if (CurrentDetectedWidget)
		{
			if (true == CurrentDetectedWidget->GetTickSpaceGeometry().IsUnderLocation(InEvent.GetScreenSpacePosition()))
			{
				if (IDragDetectable* DetectableInterface = Cast<IDragDetectable>(CurrentDetectedWidget))
				{
					DetectableInterface->NativeOnDetect(OuterWidget, InEvent);
				}

				else if (CurrentDetectedWidget->StaticClass() && CurrentDetectedWidget->StaticClass()->ImplementsInterface(UDragDetectable::StaticClass()))
				{
					IDragDetectable::Execute_OnDetect(CurrentDetectedWidget, OuterWidget, InEvent);
				}
			}

			else
			{
				if (IDragDetectable* DetectableInterface = Cast<IDragDetectable>(CurrentDetectedWidget))
				{
					DetectableInterface->NativeOnEndDetect(OuterWidget, InEvent);
				}

				else if (CurrentDetectedWidget->StaticClass() && CurrentDetectedWidget->StaticClass()->ImplementsInterface(UDragDetectable::StaticClass()))
				{
					IDragDetectable::Execute_OnEndDetect(CurrentDetectedWidget, OuterWidget, InEvent);
				}

				CurrentDetectedWidget = nullptr;

				// Find New Detectable Widget....
				for (auto& Widget : DetectableWidgets)
				{
					if (nullptr == Widget || Widget == CurrentDetectedWidget) continue;

					else if (true == Widget->GetTickSpaceGeometry().IsUnderLocation(InEvent.GetScreenSpacePosition()))
					{
						CurrentDetectedWidget = Widget;
						break;
					}
				}

				if (CurrentDetectedWidget)
				{
					if (IDragDetectable* DetectableInterface = Cast<IDragDetectable>(CurrentDetectedWidget))
					{
						DetectableInterface->NativeOnStartDetectDrag(OuterWidget, InEvent);
					}

					else if (CurrentDetectedWidget->StaticClass() && CurrentDetectedWidget->StaticClass()->ImplementsInterface(UDragDetectable::StaticClass()))
					{
						IDragDetectable::Execute_OnStartDetectDrag(CurrentDetectedWidget, OuterWidget, InEvent);
					}
				}
			}
		}

		return FReply::Handled();

	*/


}

void IDraggable::DragCancel(const FPointerEvent& InEvent)
{



	OuterPanelWidget = nullptr;
}

void IDraggable::Drop(const FPointerEvent& InEvent)
{
	/*if (CurrentDetectedWidget)
	{
		if (IDragDetectable* DetectableInterface = Cast<IDragDetectable>(CurrentDetectedWidget))
		{
			DetectableInterface->NativeOnDrop(OuterWidget, InEvent);
		}

		else if (CurrentDetectedWidget->StaticClass() && CurrentDetectedWidget->StaticClass()->ImplementsInterface(UDragDetectable::StaticClass()))
		{
			IDragDetectable::Execute_OnDrop(CurrentDetectedWidget, OuterWidget, InEvent);
		}
	}*/


	OuterPanelWidget = nullptr;
}

FVector2D IDraggable::GetClickOffset(EDragPivot InPivot, const FVector2D& InOffset) const
{
	switch (InPivot)
	{
	case EDragPivot::MouseDown:
	{

	}
		break;
	case EDragPivot::TopLeft:
	{

	}
		break;
	case EDragPivot::TopCenter:
	{

	}
		break;
	case EDragPivot::TopRight:
	{

	}
		break;
	case EDragPivot::CenterLeft:
	{

	}
		break;
	case EDragPivot::CenterCenter:
	{

	}
		break;
	case EDragPivot::CenterRight:
	{

	}
		break;
	case EDragPivot::BottomLeft:
	{

	}
		break;
	case EDragPivot::BottomCenter:
	{

	}
		break;
	case EDragPivot::BottomRight:
	{

	}
		break;
	default:
		break;
	}

	return FVector2D::ZeroVector;
}

/*
void IDraggable::NativeOnStartDrag(const FGeometry& InGeometry, const FPointerEvent& InEvent)
{
	Execute_OnStartDrag(Cast<UObject>(this), InGeometry, InEvent);
}

FReply IDraggable::DragStart(const FGeometry& InGeometry, const FPointerEvent& InEvent)
{
	UUserWidget* OuterWidget = Cast<UUserWidget>(this);

	if (nullptr == OuterWidget ||
		false == OuterWorld.IsValid())
	{
		return FReply::Unhandled();
	}

	if (true == InEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		if (true == bClick)
		{
			SetDrop(InGeometry, InEvent);

			return FReply::Unhandled()
				.ClearUserFocus()
				.ReleaseMouseLock()
				.ReleaseMouseCapture();
		}

		else
		{
			OuterPanelWidget = OuterWidget->GetParent();

			FVector2D Cursor_PixelPosition;
			FVector2D Cursor_ViewportPosition;
			USlateBlueprintLibrary::AbsoluteToViewport(OuterWorld.Get(), InEvent.GetScreenSpacePosition(), OUT Cursor_PixelPosition, OUT Cursor_ViewportPosition);

			FVector2D Widget_PixelPosition;
			FVector2D Widget_ViewportPosition;
			USlateBlueprintLibrary::AbsoluteToViewport(OuterWorld.Get(), OuterWidget->GetTickSpaceGeometry().GetAbsolutePosition(), OUT Widget_PixelPosition, OUT Widget_ViewportPosition);

			const float DPIScale = UWidgetLayoutLibrary::GetViewportScale(OuterWorld->GetGameViewport());
			const FVector2D WidgetSize = OuterWidget->GetDesiredSize() * DPIScale;
			const FVector2D ViewportSize = UWidgetLayoutLibrary::GetViewportSize(OuterWorld.Get());

			//ClickOffset = Widget_PixelPosition - Cursor_PixelPosition;
			ClickOffset = WidgetSize * 0.5f;

 			FVector2D FinalPos = Cursor_PixelPosition - ClickOffset;
			FinalPos.X = FMath::Clamp(FinalPos.X, 0.0f, ViewportSize.X - WidgetSize.X);
			FinalPos.Y = FMath::Clamp(FinalPos.Y, 0.0f, ViewportSize.Y - WidgetSize.Y);

			if (OuterPanelWidget.IsValid())
			{
				OuterPanelWidget->RemoveChild(OuterWidget);
			}

			else
			{
				OuterWidget->RemoveFromParent();
			}

			//OuterWidget->AddToViewport(INT_MAX - 1);
			OuterWidget->SetDesiredSizeInViewport(OuterWidget->GetDesiredSize());
			OuterWidget->SetPositionInViewport(FinalPos);
			
			bClick = true;

			UWidgetBlueprintLibrary::GetAllWidgetsWithInterface(OuterWorld.Get(), OUT DetectableWidgets, UDragDetectable::StaticClass(), false);

			NativeOnStartDrag(InGeometry, InEvent);

			FEventReply Reply;
			Reply = UWidgetBlueprintLibrary::Handled();
			Reply = UWidgetBlueprintLibrary::CaptureMouse(Reply, OuterWidget);
			return Reply.NativeReply;
		}
	}


	return FReply::Unhandled();
}

void IDraggable::NativeOnDrag(const FGeometry& InGeometry, const FPointerEvent& InEvent)
{
	Execute_OnDrag(Cast<UObject>(this), InGeometry, InEvent);
}

FReply IDraggable::Drag(const FGeometry& InGeometry, const FPointerEvent& InEvent)
{
	UUserWidget* OuterWidget = Cast<UUserWidget>(this);

	if (bClick && DragCursorScreenPos != InEvent.GetScreenSpacePosition() && OuterWidget)
	{
		DragCursorScreenPos = InEvent.GetScreenSpacePosition();
		//UUserWidget* OuterWidget = Cast<UUserWidget>(this);

		FVector2D Cursor_PixelPosition;
		FVector2D Cursor_ViewportPosition;
		USlateBlueprintLibrary::AbsoluteToViewport(OuterWorld.Get(), InEvent.GetScreenSpacePosition(), OUT Cursor_PixelPosition, OUT Cursor_ViewportPosition);

		FVector2D Widget_PixelPosition;
		FVector2D Widget_ViewportPosition;
		USlateBlueprintLibrary::AbsoluteToViewport(OuterWorld.Get(), OuterWidget->GetTickSpaceGeometry().GetAbsolutePosition(), OUT Widget_PixelPosition, OUT Widget_ViewportPosition);

		const float DPIScale = UWidgetLayoutLibrary::GetViewportScale(OuterWorld->GetGameViewport());
		const FVector2D WidgetSize = OuterWidget->GetDesiredSize() * DPIScale;

		FVector2D WidgetPos = Cursor_PixelPosition - ClickOffset;
		const FVector2D ViewportSize = UWidgetLayoutLibrary::GetViewportSize(OuterWorld.Get());

		FVector2D FinalPos;
		FinalPos.X = FMath::Clamp(WidgetPos.X, 0.0f, ViewportSize.X - WidgetSize.X);
		FinalPos.Y = FMath::Clamp(WidgetPos.Y, 0.0f, ViewportSize.Y - WidgetSize.Y);

		OuterWidget->SetPositionInViewport(FinalPos);

		NativeOnDrag(InGeometry, InEvent);

		if (nullptr == CurrentDetectedWidget)
		{
			for (auto& Widget : DetectableWidgets)
			{
				if (nullptr == Widget) continue;

				else if (true == Widget->GetTickSpaceGeometry().IsUnderLocation(InEvent.GetScreenSpacePosition()))
				{
					CurrentDetectedWidget = Widget;
					break;
				}
			}

			if (IDragDetectable* DetectableInterface = Cast<IDragDetectable>(CurrentDetectedWidget))
			{
				DetectableInterface->NativeOnStartDetectDrag(OuterWidget, InEvent);
			}

			else if (CurrentDetectedWidget->StaticClass() && CurrentDetectedWidget->StaticClass()->ImplementsInterface(UDragDetectable::StaticClass()))
			{
				IDragDetectable::Execute_OnStartDetectDrag(CurrentDetectedWidget, OuterWidget, InEvent);
			}
		}

		else if (CurrentDetectedWidget)
		{
			if (true == CurrentDetectedWidget->GetTickSpaceGeometry().IsUnderLocation(InEvent.GetScreenSpacePosition()))
			{
				if (IDragDetectable* DetectableInterface = Cast<IDragDetectable>(CurrentDetectedWidget))
				{
					DetectableInterface->NativeOnDetect(OuterWidget, InEvent);
				}

				else if (CurrentDetectedWidget->StaticClass() && CurrentDetectedWidget->StaticClass()->ImplementsInterface(UDragDetectable::StaticClass()))
				{
					IDragDetectable::Execute_OnDetect(CurrentDetectedWidget, OuterWidget, InEvent);
				}
			}

			else
			{
				if (IDragDetectable* DetectableInterface = Cast<IDragDetectable>(CurrentDetectedWidget))
				{
					DetectableInterface->NativeOnEndDetect(OuterWidget, InEvent);
				}

				else if (CurrentDetectedWidget->StaticClass() && CurrentDetectedWidget->StaticClass()->ImplementsInterface(UDragDetectable::StaticClass()))
				{
					IDragDetectable::Execute_OnEndDetect(CurrentDetectedWidget, OuterWidget, InEvent);
				}

				CurrentDetectedWidget = nullptr;

				// Find New Detectable Widget....
				for (auto& Widget : DetectableWidgets)
				{
					if (nullptr == Widget || Widget == CurrentDetectedWidget) continue;

					else if (true == Widget->GetTickSpaceGeometry().IsUnderLocation(InEvent.GetScreenSpacePosition()))
					{
						CurrentDetectedWidget = Widget;
						break;
					}
				}

				if (CurrentDetectedWidget)
				{
					if (IDragDetectable* DetectableInterface = Cast<IDragDetectable>(CurrentDetectedWidget))
					{
						DetectableInterface->NativeOnStartDetectDrag(OuterWidget, InEvent);
					}

					else if (CurrentDetectedWidget->StaticClass() && CurrentDetectedWidget->StaticClass()->ImplementsInterface(UDragDetectable::StaticClass()))
					{
						IDragDetectable::Execute_OnStartDetectDrag(CurrentDetectedWidget, OuterWidget, InEvent);
					}
				}
			}
		}

		return FReply::Handled();

	}

	return  FReply::Unhandled();
}

void IDraggable::NativeOnEndDrag(const FGeometry& InGeometry, const FPointerEvent& InEvent)
{
	Execute_OnEndDrag(Cast<UObject>(this), InGeometry, InEvent);
}

void IDraggable::NativeOnDrop(UPanelSlot* InPanelSlot)
{
	Execute_OnDrop(Cast<UObject>(this), InPanelSlot);
}

FReply IDraggable::DragEnd(const FGeometry& InGeometry, const FPointerEvent& InEvent)
{
	if (false == bUseToogle)
	{
		SetDrop(InGeometry, InEvent);

		return FReply::Unhandled()
			.ClearUserFocus()
			.ReleaseMouseLock()
			.ReleaseMouseCapture();
	}

	return FReply::Unhandled();
}

void IDraggable::SetDrop(const FGeometry& InGeometry, const FPointerEvent& InEvent)
{
	UUserWidget* OuterWidget = Cast<UUserWidget>(this);

	if (OuterWidget)
	{
		bClick = false;
		ClickOffset = FVector2D::Zero();
		DragCursorScreenPos = FVector2D::Zero();

		NativeOnEndDrag(InGeometry, InEvent);

		if (CurrentDetectedWidget)
		{
			if (IDragDetectable* DetectableInterface = Cast<IDragDetectable>(CurrentDetectedWidget))
			{
				DetectableInterface->NativeOnDrop(OuterWidget, InEvent);
			}

			else if (CurrentDetectedWidget->StaticClass() && CurrentDetectedWidget->StaticClass()->ImplementsInterface(UDragDetectable::StaticClass()))
			{
				IDragDetectable::Execute_OnDrop(CurrentDetectedWidget, OuterWidget, InEvent);
			}
		}

		// failed drop
		else if (OuterPanelWidget.IsValid())
		{
			UPanelSlot* PanelSlot = OuterPanelWidget->AddChild(OuterWidget);
			NativeOnDrop(PanelSlot);
		}

		CurrentDetectedWidget = nullptr;
	}
}
*/

void IDraggable::SetEvent(UWidget* InWidget)
{
	if (!ensure(InWidget))
	{
		return;
	}

	EventWidget = InWidget;

	TSharedPtr<SWidget> SlateWidget = EventWidget->TakeWidget();
	if (SlateWidget.IsValid())
	{
		SlateWidget->SetOnMouseButtonDown(FPointerEventHandler::CreateRaw(this, &IDraggable::MouseButtonDown));
		//SlateWidget->SetOnMouseDoubleClick(FPointerEventHandler::CreateRaw(this, &IDraggable::DragStart));
		//SlateWidget->SetOnMouseMove(FPointerEventHandler::CreateRaw(this, &IDraggable::Drag));
		//SlateWidget->SetOnMouseButtonUp(FPointerEventHandler::CreateRaw(this, &IDraggable::DragEnd));
		//SlateWidget->SetOnMouseEnter(FNoReplyPointerEventHandler::CreateRaw(this, &IDraggable::Enter));
		//SlateWidget->SetOnMouseLeave(FSimpleNoReplyPointerEventHandler::CreateRaw(this, &IDraggable::Leave));
	}

}
