// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "Draggable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UDraggable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */

class UWorld;
class APlayerController;

class UPanelWidget;
class UImage;
class UBorder;
class UWidget;
class UUserWidget;

enum class EDragPivot : uint8;

class ECLIPSE_API IDraggable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.

	friend class UDragPayload;

public:
	void SetWorld(UWorld* InWorld);
	void SetOwningController(APlayerController* InController);
	void Reset();
	void SetEventFromImage(UImage* InImage);
	void SetEventFromBorder(UBorder* InBorder);

	void SetEnableToggle(bool bIsEnable);

	UDragPayload* CreateDragPayload(TFunction<void(UDragPayload*)> InFunc);


private:
	FReply MouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InEvent);

	void Drag(UUserWidget* InVisualWidget, EDragPivot InPivot, const FVector2D& InOffset, const FPointerEvent& InEvent);
	void DragCancel(const FPointerEvent& InEvent);
	void Drop(const FPointerEvent& InEvent);

	FVector2D GetClickOffset(EDragPivot InPivot, const FVector2D& InOffset) const;

private:
	void SetEvent(UWidget* InWidget);

private:
	TWeakObjectPtr<UWorld> OuterWorld = nullptr;
	TWeakObjectPtr<UWidget> EventWidget = nullptr;
	TWeakObjectPtr<UPanelWidget> OuterPanelWidget = nullptr;

	TArray<UUserWidget*> DetectableWidgets;

	UUserWidget* CurrentDetectedWidget = nullptr;
};
