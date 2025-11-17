// Fill out your copyright notice in the Description page of Project Settings.


#include "Draggable.h"

#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/Widget.h"

#include "Blueprint/SlateBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

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
			SlateWidget->SetOnMouseLeave(FSimpleNoReplyPointerEventHandler());
		}
	}

	EventWidget.Reset();
	OuterUserWidget.Reset();
}

void IDraggable::SetEventFromImage(UImage* InImage)
{
	SetEvent(InImage);
}

void IDraggable::SetEventFromBorder(UBorder* InBorder)
{
	SetEvent(InBorder);
}

void IDraggable::NativeOnDetectedDrag(const FGeometry& InGeometry, const FPointerEvent& InEvent)
{
	Execute_OnDetectedDrag(Cast<UObject>(this), InGeometry, InEvent);
}

FReply IDraggable::DetectedDrag(const FGeometry& InGeometry, const FPointerEvent& InEvent)
{
	if (false == OuterWorld.IsValid() 
		|| false == OuterUserWidget.IsValid())
	{
		return FReply::Unhandled();
	}

	OuterPanelWidget = OuterUserWidget->GetParent();

	const float DPIScale = UWidgetLayoutLibrary::GetViewportScale(OuterWorld->GetGameViewport());
	const FVector2D WidgetSize = OuterUserWidget->GetDesiredSize() * DPIScale;

	// 커서 위치
	FVector2D Cursor_PixelPosition;
	FVector2D Cursor_ViewportPosition;
	USlateBlueprintLibrary::AbsoluteToViewport(OuterWorld.Get(), InEvent.GetScreenSpacePosition(), OUT Cursor_PixelPosition, OUT Cursor_ViewportPosition);
	
	// 위젯 위치 (Top Left)
	FVector2D Widget_PixelPosition;
	FVector2D Widget_ViewportPosition;
	USlateBlueprintLibrary::AbsoluteToViewport(OuterWorld.Get(), OuterUserWidget->GetTickSpaceGeometry().GetAbsolutePosition(), OUT Widget_PixelPosition, OUT Widget_ViewportPosition);
	
	ClickOffset = Widget_PixelPosition - Cursor_PixelPosition;
	
	FVector2D FinalPos = Cursor_PixelPosition + ClickOffset;

	if (OuterPanelWidget.IsValid())
	{
		OuterPanelWidget->RemoveChild(OuterUserWidget.Get());
	}

	else
	{
		OuterUserWidget->RemoveFromParent();
	}

	OuterUserWidget->AddToViewport(INT_MAX - 1);
	OuterUserWidget->SetDesiredSizeInViewport(OuterUserWidget->GetDesiredSize());
	OuterUserWidget->SetPositionInViewport(FinalPos);

	bDrag = true;
	NativeOnDetectedDrag(InGeometry, InEvent);

	return FReply::Handled()
		.CaptureMouse(EventWidget->TakeWidget())
		.UseHighPrecisionMouseMovement(OuterUserWidget->TakeWidget());
}

void IDraggable::NativeOnDrag(const FGeometry& InGeometry, const FPointerEvent& InEvent)
{
	Execute_OnDrag(Cast<UObject>(this), InGeometry, InEvent);
}

FReply IDraggable::Drag(const FGeometry& InGeometry, const FPointerEvent& InEvent)
{
	if (false == OuterWorld.IsValid()
		|| false == OuterUserWidget.IsValid()
		|| false == bDrag)
	{
		return FReply::Unhandled();
	}

	FVector2D Cursor_PixelPosition;
	FVector2D Cursor_ViewportPosition;
	USlateBlueprintLibrary::AbsoluteToViewport(OuterWorld.Get(), InEvent.GetScreenSpacePosition(), OUT Cursor_PixelPosition, OUT Cursor_ViewportPosition);

	FVector2D Widget_PixelPosition;
	FVector2D Widget_ViewportPosition;
	USlateBlueprintLibrary::AbsoluteToViewport(OuterWorld.Get(), OuterUserWidget->GetTickSpaceGeometry().GetAbsolutePosition(), OUT Widget_PixelPosition, OUT Widget_ViewportPosition);
	
	const float DPIScale = UWidgetLayoutLibrary::GetViewportScale(OuterWorld->GetGameViewport());
	const FVector2D WidgetSize = OuterUserWidget->GetDesiredSize() * DPIScale;
	
	FVector2D FinalPos = Cursor_PixelPosition + ClickOffset;
	const FVector2D ViewportSize = UWidgetLayoutLibrary::GetViewportSize(OuterWorld.Get());

	//FinalPos.X = FMath::Clamp(FinalPos.X, 0.0f, ViewportSize.X - WidgetSize.X);
	//FinalPos.Y = FMath::Clamp(FinalPos.Y, 0.0f, ViewportSize.Y - WidgetSize.Y);
	OuterUserWidget->SetPositionInViewport(FinalPos);

	NativeOnDrag(InGeometry, InEvent);

	return FReply::Handled();
}

void IDraggable::NativeOnDrop(const FGeometry& InGeometry, const FPointerEvent& InEvent)
{
	Execute_OnDrop(Cast<UObject>(this), InGeometry, InEvent);
}

FReply IDraggable::Drop(const FGeometry& InGeometry, const FPointerEvent& InEvent)
{
	if (false == OuterUserWidget.IsValid())
	{
		return FReply::Unhandled();
	}

	ClickOffset = FVector2D::Zero();

	if (OuterPanelWidget.IsValid())
	{
		UPanelSlot* PanelSlot = OuterPanelWidget->AddChild(OuterUserWidget.Get());
	}
	bDrag = false;

	// TODO : PanelSlot을 넘겨주기?
	NativeOnDrop(InGeometry, InEvent);

	return FReply::Unhandled()
		.ReleaseMouseCapture();
}

void IDraggable::Leave(const FPointerEvent& InEvent)
{
	//bDrag = false;
}

void IDraggable::SetEvent(UWidget* InWidget)
{
	if (!ensure(InWidget))
	{
		return;
	}

	EventWidget = InWidget;

	OuterUserWidget = Cast<UUserWidget>(this);

	TSharedPtr<SWidget> SlateWidget = EventWidget->TakeWidget();
	if (SlateWidget.IsValid())
	{
		SlateWidget->SetOnMouseButtonDown(FPointerEventHandler::CreateRaw(this, &IDraggable::DetectedDrag));
		SlateWidget->SetOnMouseDoubleClick(FPointerEventHandler::CreateRaw(this, &IDraggable::DetectedDrag));
		SlateWidget->SetOnMouseMove(FPointerEventHandler::CreateRaw(this, &IDraggable::Drag));
		SlateWidget->SetOnMouseButtonUp(FPointerEventHandler::CreateRaw(this, &IDraggable::Drop));
		SlateWidget->SetOnMouseLeave(FSimpleNoReplyPointerEventHandler::CreateRaw(this, &IDraggable::Leave));
	}

}
