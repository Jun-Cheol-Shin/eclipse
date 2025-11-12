// Fill out your copyright notice in the Description page of Project Settings.


#include "GridBasedListEntry.h"

#include "Components/SizeBox.h"
#include "Components/Image.h"
#include "Materials/MaterialInstanceDynamic.h"

void UGridBasedListEntry::NativeOnListItemObjectSet(UGridBasedListItem* ListItemObject)
{
    if (nullptr == ListItemObject)
    {
        return;
    }

    IGridBasedObjectListEntry::NativeOnListItemObjectSet(ListItemObject);


    if (SizeBox)
    {
        SizeBox->SetWidthOverride(GetSlotSize() * ListItemObject->TileSize.X);
        SizeBox->SetHeightOverride(GetSlotSize() * ListItemObject->TileSize.Y);
    }

}

void UGridBasedListEntry::NativeOnItemSelectionChanged(bool bIsSelected)
{
    IGridBasedObjectListEntry::NativeOnItemSelectionChanged(bIsSelected);


}

void UGridBasedListEntry::NativeOnEntryReleased()
{
    IGridBasedObjectListEntry::NativeOnEntryReleased();


}

void UGridBasedListEntry::NativeConstruct()
{
    Super::NativeConstruct();
}

void UGridBasedListEntry::SynchronizeProperties()
{
    Super::SynchronizeProperties();


}