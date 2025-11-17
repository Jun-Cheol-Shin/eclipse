// Fill out your copyright notice in the Description page of Project Settings.

#include "GridBasedObjectListEntry.h"
#include "GridBasedListView.h"


// Add default functionality here for any IGridBasedListEntry functions that are not pure virtual.
void IGridBasedObjectListEntry::NativeOnListItemObjectSet(UGridBasedListItem* ListItemObject)
{
	Execute_OnListItemObjectSet(Cast<UObject>(this), ListItemObject);
}

void IGridBasedObjectListEntry::NativeOnItemSelectionChanged(bool bIsSelected)
{
	Execute_OnItemSelectionChanged(Cast<UObject>(this), bIsSelected);
}


void IGridBasedObjectListEntry::NativeOnEntryReleased()
{
	Execute_OnEntryReleased(Cast<UObject>(this));

	OwningListView.Reset();
}

UGridBasedListView* IGridBasedObjectListEntry::GetOwningListView() const
{
	if (OwningListView.IsValid())
	{
		return OwningListView.Get();
	}

	return nullptr;
}

float IGridBasedObjectListEntry::GetSlotSize() const
{
	if (OwningListView.IsValid())
	{
		return OwningListView->GetSlotSize();
	}

	return 0.f;
}