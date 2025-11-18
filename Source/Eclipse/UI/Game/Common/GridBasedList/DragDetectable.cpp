// Fill out your copyright notice in the Description page of Project Settings.


#include "DragDetectable.h"


// Add default functionality here for any IDropDetectable functions that are not pure virtual.

void IDragDetectable::NativeOnDetectedDrag(UUserWidget* InDraggingWidget, const FPointerEvent& InEvent)
{
	Execute_OnDetectedDrag(Cast<UObject>(this), InDraggingWidget, InEvent);
}

void IDragDetectable::NativeOnDetect(UUserWidget* InDraggingWidget, const FPointerEvent& InEvent)
{
	Execute_OnDetect(Cast<UObject>(this), InDraggingWidget, InEvent);
}

void IDragDetectable::NativeOnEndDetect(UUserWidget* InDraggingWidget, const FPointerEvent& InEvent)
{
	Execute_OnEndDetect(Cast<UObject>(this), InDraggingWidget, InEvent);
}

void IDragDetectable::NativeOnDrop(UUserWidget* InDraggingWidget, const FPointerEvent& InEvent)
{
	Execute_OnDrop(Cast<UObject>(this), InDraggingWidget, InEvent);
}
