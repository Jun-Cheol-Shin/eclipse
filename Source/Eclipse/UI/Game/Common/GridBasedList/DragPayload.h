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

	friend class IDraggable;

public:
	//UUserWidget* GetDraggableWidget() const;

protected:
	virtual void BeginDestroy() override;

	//virtual void Drop_Implementation(const FPointerEvent& PointerEvent);
	virtual void DragCancelled_Implementation(const FPointerEvent& PointerEvent);
	virtual void Dragged_Implementation(const FPointerEvent& PointerEvent);

private:
	void Set(UUserWidget* InDraggable);

private:
	TWeakObjectPtr<UUserWidget> DraggableWidget = nullptr;
};
