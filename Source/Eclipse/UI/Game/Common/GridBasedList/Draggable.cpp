// Fill out your copyright notice in the Description page of Project Settings.


#include "Draggable.h"
#include "DragPayload.h"
#include "DragDetectable.h"

#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/Widget.h"
#include "Components/PanelWidget.h"

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

void IDraggable::NativeOnDragCancel(UPanelSlot* InPanelSlot)
{
	Execute_OnDragCancel(Cast<UObject>(this), InPanelSlot);
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

FReply IDraggable::MouseMove(const FGeometry& InGeometry, const FPointerEvent& InEvent)
{
	UE_LOG(LogTemp, Warning, TEXT("Move!"));



	return FReply::Handled();
}

void IDraggable::Drag(EDragPivot InPivot, const FVector2D& InOffset, const FPointerEvent& InEvent)
{
	UUserWidget* OuterWidget = Cast<UUserWidget>(this);
	if (nullptr == OuterWidget)
	{
		return;
	}

	FVector2D Cursor_PixelPosition;
	FVector2D Cursor_ViewportPosition;
	USlateBlueprintLibrary::AbsoluteToViewport(OuterWorld.Get(), InEvent.GetScreenSpacePosition(), OUT Cursor_PixelPosition, OUT Cursor_ViewportPosition);

	FVector2D Widget_PixelPosition;
	FVector2D Widget_ViewportPosition;
	USlateBlueprintLibrary::AbsoluteToViewport(OuterWorld.Get(), OuterWidget->GetTickSpaceGeometry().GetAbsolutePosition(), OUT Widget_PixelPosition, OUT Widget_ViewportPosition);

	const float DPIScale = UWidgetLayoutLibrary::GetViewportScale(OuterWorld->GetGameViewport());
	const FVector2D WidgetSize = OuterWidget->GetDesiredSize() * DPIScale;
	const FVector2D ViewportSize = UWidgetLayoutLibrary::GetViewportSize(OuterWorld.Get());

	if (ClickOffset == FVector2D(-1, -1))
	{
		ClickOffset = GetClickOffset(Widget_PixelPosition, Cursor_PixelPosition, WidgetSize, InPivot);
	}

	FVector2D CursorPos = Cursor_PixelPosition - ClickOffset;

	FVector2D MaxPosVP = ViewportSize - WidgetSize;
	MaxPosVP.X = FMath::Max(0.f, MaxPosVP.X);
	MaxPosVP.Y = FMath::Max(0.f, MaxPosVP.Y);

	CursorPos.X = FMath::Clamp(CursorPos.X, 0.f, MaxPosVP.X);
	CursorPos.Y = FMath::Clamp(CursorPos.Y, 0.f, MaxPosVP.Y);

	OuterWidget->SetPositionInViewport(CursorPos);


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

}

void IDraggable::DragCancel(const FPointerEvent& InEvent)
{
	UUserWidget* OuterWidget = Cast<UUserWidget>(this);

	ClickOffset = FVector2D(-1, -1);

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
	else if (nullptr != OuterPanelWidget)
	{
		if (UPanelSlot* PanelSlot = OuterPanelWidget->AddChild(OuterWidget))
		{
			NativeOnDragCancel(PanelSlot);
		}
	}

	OuterPanelWidget = nullptr;
	CurrentDetectedWidget = nullptr;

}

FVector2D IDraggable::GetClickOffset(const FVector2D& InWidgetPos, const FVector2D& InCursorPos, const FVector2D& InWidgetSize, EDragPivot InPivot) const
{
	switch (InPivot)
	{
	case EDragPivot::MouseDown:
	{
		return InCursorPos - InWidgetPos;
	}
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
		SlateWidget->SetOnMouseMove(FPointerEventHandler::CreateRaw(this, &IDraggable::MouseMove));

		//SlateWidget->SetOnMouseDoubleClick(FPointerEventHandler::CreateRaw(this, &IDraggable::DragStart));

		//SlateWidget->SetOnMouseButtonUp(FPointerEventHandler::CreateRaw(this, &IDraggable::DragEnd));
		//SlateWidget->SetOnMouseEnter(FNoReplyPointerEventHandler::CreateRaw(this, &IDraggable::Enter));
		//SlateWidget->SetOnMouseLeave(FSimpleNoReplyPointerEventHandler::CreateRaw(this, &IDraggable::Leave));
	}

}
