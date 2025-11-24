// Fill out your copyright notice in the Description page of Project Settings.


#include "DragPayload.h"
#include "Draggable.h"

#include "Blueprint/UserWidget.h"

void UDragPayload::Set(UUserWidget* InDraggableWidget)
{
	DraggableWidget = InDraggableWidget;
}

void UDragPayload::BeginDestroy()
{
	Super::BeginDestroy();

	DraggableWidget.Reset();
}

void UDragPayload::Dragged_Implementation(const FPointerEvent& PointerEvent)
{
	if (IDraggable* Draggable = Cast<IDraggable>(DraggableWidget))
	{
		if (nullptr == DuplicatedWidget)
		{
			DuplicatedWidget = DuplicateObject<UUserWidget>(DragVisualWidgetClass, this);

			if (DuplicatedWidget)
			{
				DuplicatedWidget->AddToViewport(INT_MAX - 1);
			}
		}

		Draggable->Drag(DuplicatedWidget, Pivot, Offset, PointerEvent);
	}
}

void UDragPayload::Drop_Implementation(const FPointerEvent& PointerEvent)
{
	if (IDraggable* Draggable = Cast<IDraggable>(DraggableWidget))
	{
		if (DuplicatedWidget)
		{
			DuplicatedWidget->RemoveFromParent();
			DuplicatedWidget = nullptr;
		}

		Draggable->Drop(PointerEvent);
	}
}

void UDragPayload::DragCancelled_Implementation(const FPointerEvent& PointerEvent)
{
	if (IDraggable* Draggable = Cast<IDraggable>(DraggableWidget))
	{
		if (DuplicatedWidget)
		{
			DuplicatedWidget->RemoveFromParent();
			DuplicatedWidget = nullptr;
		}

		Draggable->DragCancel(PointerEvent);
	}
}

