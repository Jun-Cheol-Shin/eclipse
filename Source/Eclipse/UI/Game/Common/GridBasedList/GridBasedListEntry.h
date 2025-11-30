// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "GameplayTagContainer.h"
#include "GridBasedFootprint.h"
#include "GridBasedObjectListEntry.h"
#include "Draggable.h"
#include "GridBasedListEntry.generated.h"


class USizeBox;
class UDragDropOperation;

UCLASS()
class UGridBasedListItem : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	FIntPoint TopLeftPos{};

	UPROPERTY(EditAnywhere)
	FIntPoint TileSize{};

	UPROPERTY(EditAnywhere)
	TArray<FIntPoint> HiddenIndexes{};

};


UCLASS(Abstract, Blueprintable, MinimalAPI)
class UGridBasedListEntry : public UCommonUserWidget, public IGridBasedObjectListEntry, public IDraggable
{
	GENERATED_BODY()

protected:
	// IGridBasedObjectListEntry
	virtual void NativeOnListItemObjectSet(UGridBasedListItem* ListItemObject) override;
	virtual void NativeOnItemSelectionChanged(bool bIsSelected) override;
	virtual void NativeOnEntryReleased() override;
	// End IGridBasedObjectListEntry

	// UUserWidget Event
	virtual void NativeConstruct() override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;	
	virtual void SynchronizeProperties() override;
	// End UUserWidget Event

	// Draggable Event
	virtual void NativeOnDragCancel(UPanelSlot* InSlot) override;
	// end Draggable Event

private:
	UPROPERTY(meta = (BindWidget))
	USizeBox* SizeBox = nullptr;

	UPROPERTY(meta = (BindWidget))
	UGridBasedFootprint* Footprint = nullptr;
};
