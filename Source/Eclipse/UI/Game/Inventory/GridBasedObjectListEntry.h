// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "GridBasedObjectListEntry.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UGridBasedObjectListEntry : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ECLIPSE_API IGridBasedObjectListEntry
{
	GENERATED_BODY()

	friend class UGridBasedInventoryList;

protected:
	/** Follows the same pattern as the NativeOn[X] methods in UUserWidget - super calls are expected in order to route the event to BP. */
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject);
	virtual void NativeOnItemSelectionChanged(bool bIsSelected);
	virtual void NativeOnEntryReleased();

	/** Called when this entry is assigned a new item object to represent by the owning list view */
	UFUNCTION(BlueprintImplementableEvent, Category = ObjectListEntry, meta = (DisplayName = "On Entry Item Object Set"))
	void OnListItemObjectSet(UObject* ListItemObject);

	/** Called when the selection state of the item represented by this entry changes. */
	UFUNCTION(BlueprintImplementableEvent, Category = ObjectListEntry, meta = (DisplayName = "On Item Selection Changed"))
	void OnItemSelectionChanged(bool bIsSelected);

	/** Called when this entry is released from the owning table and no longer represents any list item */
	UFUNCTION(BlueprintImplementableEvent, Category = ObjectListEntry, meta = (DisplayName = "On Entry Released"))
	void OnEntryReleased();

private:
	FVector2D ItemPosition;
	FVector2D ItemSize;	
};
