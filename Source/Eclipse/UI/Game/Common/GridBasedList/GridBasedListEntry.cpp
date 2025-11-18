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
        float Width = GetSlotSize() * static_cast<float>(ListItemObject->TileSize.X);
        float Height = GetSlotSize() * static_cast<float>(ListItemObject->TileSize.Y);

        SizeBox->SetWidthOverride(Width);
        SizeBox->SetHeightOverride(Height);
    }

    SetWorld(GetWorld());
    SetEventFromImage(Footprint->FootprintImage);
}

void UGridBasedListEntry::NativeOnItemSelectionChanged(bool bIsSelected)
{
    IGridBasedObjectListEntry::NativeOnItemSelectionChanged(bIsSelected);


}

void UGridBasedListEntry::NativeOnEntryReleased()
{
    IGridBasedObjectListEntry::NativeOnEntryReleased();

    Reset();
}

void UGridBasedListEntry::NativeOnStartDrag(const FGeometry& InGeometry, const FPointerEvent& InEvent)
{
    IDraggable::NativeOnStartDrag(InGeometry, InEvent);


}

void UGridBasedListEntry::NativeOnDrag(const FGeometry& InGeometry, const FPointerEvent& InEvent)
{
    IDraggable::NativeOnDrag(InGeometry, InEvent);


}

void UGridBasedListEntry::NativeOnEndDrag(const FGeometry& InGeometry, const FPointerEvent& InEvent)
{
    IDraggable::NativeOnEndDrag(InGeometry, InEvent);


}

void UGridBasedListEntry::NativeConstruct()
{
    Super::NativeConstruct();
}

void UGridBasedListEntry::SynchronizeProperties()
{
    Super::SynchronizeProperties();


}