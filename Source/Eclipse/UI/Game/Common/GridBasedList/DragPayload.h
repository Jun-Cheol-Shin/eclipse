// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "DragPayload.generated.h"

/**
 * 
 */

class IDraggable;

UCLASS()
class ECLIPSE_API UDragPayload : public UDragDropOperation
{
	GENERATED_BODY()

public:
	const UUserWidget* DragVisualWidgetClass = nullptr;

	void Set(UUserWidget* InDraggableWidget);

protected:
	virtual void BeginDestroy() override;

	virtual void Drop_Implementation(const FPointerEvent& PointerEvent);
	virtual void DragCancelled_Implementation(const FPointerEvent& PointerEvent);
	virtual void Dragged_Implementation(const FPointerEvent& PointerEvent);


private:
	TWeakObjectPtr<UUserWidget> DraggableWidget = nullptr;

	UPROPERTY()
	UUserWidget* DuplicatedWidget = nullptr;
};
