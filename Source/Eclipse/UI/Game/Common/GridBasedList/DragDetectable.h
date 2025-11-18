// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "DragDetectable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UDragDetectable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ECLIPSE_API IDragDetectable
{
	GENERATED_BODY()

	friend class IDraggable;

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
protected:
	virtual void NativeOnDetectedDrag(UUserWidget* InDraggingWidget, const FPointerEvent& InEvent);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Drag Detectable Functions", DisplayName = "On Detected Drag")
	void OnDetectedDrag(UUserWidget* InDraggingWidget, const FPointerEvent& InEvent);

	virtual void NativeOnDetect(UUserWidget* InDraggingWidget, const FPointerEvent& InEvent);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Drag Detectable Functions", DisplayName = "On Detect")
	void OnDetect(UUserWidget* InDraggingWidget, const FPointerEvent& InEvent);

	virtual void NativeOnEndDetect(UUserWidget* InDraggingWidget, const FPointerEvent& InEvent);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Drag Detectable Functions", DisplayName = "On End Detect")
	void OnEndDetect(UUserWidget* InDraggingWidget, const FPointerEvent& InEvent);


	virtual void NativeOnDrop(UUserWidget* InDraggingWidget, const FPointerEvent& InEvent);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Drag Detectable Functions", DisplayName = "On Drop")
	void OnDrop(UUserWidget* InDraggingWidget, const FPointerEvent& InEvent);
	
};
