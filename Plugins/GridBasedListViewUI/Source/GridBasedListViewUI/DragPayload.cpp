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
		TWeakObjectPtr<UUserWidget> MoveWidget = Draggable->MoveWidget;
		if (false == MoveWidget.IsValid())
		{
			return;
		}

		if (UPanelWidget* PanelWidget = MoveWidget->GetParent())
		{
			PanelWidget->RemoveChild(MoveWidget.Get());
		}

		if (false == MoveWidget->IsInViewport())
		{
			MoveWidget->AddToViewport(0);
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

