// Fill out your copyright notice in the Description page of Project Settings.


#include "DragPayload.h"
#include "Draggable.h"

#include "Components/PanelWidget.h"
#include "Blueprint/UserWidget.h"

void UDragPayload::Set(UUserWidget* InDraggable)
{
	DraggableWidget = InDraggable;
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
		if (UPanelWidget* PanelWidget = DraggableWidget->GetParent())
		{
			PanelWidget->RemoveChild(DraggableWidget.Get());
		}

		if (false == DraggableWidget->IsInViewport())
		{
			DraggableWidget->AddToViewport(0);
		}

		Draggable->Drag(Pivot, Offset, PointerEvent);
	}
}


void UDragPayload::DragCancelled_Implementation(const FPointerEvent& PointerEvent)
{
	if (IDraggable* Draggable = Cast<IDraggable>(DraggableWidget))
	{
		Draggable->DragCancel(PointerEvent);
	}
}

