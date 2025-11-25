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

protected:
	virtual void NativeOnDragCancel(UPanelSlot* InPanelSlot);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Draggable Functions", DisplayName = "On Drag Cancel")
	void OnDragCancel(UPanelSlot* InPanelSlot);

private:
	FReply MouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InEvent);
	FReply MouseMove(const FGeometry& InGeometry, const FPointerEvent& InEvent);

	void Drag(EDragPivot InPivot, const FVector2D& InOffset, const FPointerEvent& InEvent);
	void DragCancel(const FPointerEvent& InEvent);
	//void Drop(const FPointerEvent& InEvent);

	FVector2D GetClickOffset(const FVector2D& InWidgetPos, const FVector2D& InCursorPos, const FVector2D& InWidgetSize, EDragPivot InPivot) const;

private:
	void SetEvent(UWidget* InWidget);

private:
	TWeakObjectPtr<UWorld> OuterWorld = nullptr;
	TWeakObjectPtr<UWidget> EventWidget = nullptr;

	UUserWidget* CurrentDetectedWidget = nullptr;
	TArray<UUserWidget*> DetectableWidgets;

private:
	FVector2D ClickOffset = FVector2D(-1, -1);
	UPanelWidget* OuterPanelWidget = nullptr;

	UUserWidget* VisualWidget = nullptr;
};
