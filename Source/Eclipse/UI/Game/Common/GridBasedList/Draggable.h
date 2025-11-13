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

class ECLIPSE_API IDraggable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.

public:
	void SetWorld(UWorld* InWorld);
	void SetOwningController(APlayerController* InController);
	void Reset();

	void SetEventFromImage(UImage* InImage);
	void SetEventFromBorder(UBorder* InBorder);


protected:
	virtual void NativeOnDetectedDrag(const FGeometry& InGeometry, const FPointerEvent& InEvent);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Drag Widget Functions", DisplayName = "On Detected Drag")
	void OnDetectedDrag(const FGeometry& InGeometry, const FPointerEvent& InEvent);

	virtual void NativeOnDrag(const FGeometry& InGeometry, const FPointerEvent& InEvent);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Drag Widget Functions", DisplayName = "On Drag")
	void OnDrag(const FGeometry& InGeometry, const FPointerEvent& InEvent);

	virtual void NativeOnDrop(const FGeometry& InGeometry, const FPointerEvent& InEvent);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Drag Widget Functions", DisplayName = "On Drop")
	void OnDrop(const FGeometry& InGeometry, const FPointerEvent& InEvent);

private:
	FReply DetectedDrag(const FGeometry& InGeometry, const FPointerEvent& InEvent);
	FReply Drag(const FGeometry& InGeometry, const FPointerEvent& InEvent);
	FReply Drop(const FGeometry& InGeometry, const FPointerEvent& InEvent);

private:
	void SetEvent(UWidget* InWidget);

private:
	bool bDrag = false;
	FVector2D ClickOffset {};

	TWeakObjectPtr<UWorld> OuterWorld = nullptr;
	TWeakObjectPtr<UWidget> EventWidget = nullptr;

	TWeakObjectPtr<UUserWidget> OuterUserWidget = nullptr;
	TWeakObjectPtr<UPanelWidget> OuterPanelWidget = nullptr;
};
