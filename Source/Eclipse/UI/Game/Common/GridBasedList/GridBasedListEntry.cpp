// Fill out your copyright notice in the Description page of Project Settings.


#include "GridBasedListEntry.h"

#include "Components/SizeBox.h"
#include "Components/Image.h"
#include "Components/CanvasPanelSlot.h"
#include "Materials/MaterialInstanceDynamic.h"

void UGridBasedListEntry::NativeOnListItemObjectSet(UGridBasedListItem* ListItemObject)
{
    IGridBasedObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

    if (nullptr == ListItemObject)
    {
        return;
    }

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

void UGridBasedListEntry::NativeOnDrop(UPanelSlot* InPanelSlot)
{
    IDraggable::NativeOnDrop(InPanelSlot);

    // Failed Drop on detectable widget..

    const UGridBasedListItem* OwningItem = GetOwningListItem();
    if (nullptr == OwningItem)
    {
        return;
    }

    UCanvasPanelSlot* PanelSlot = Cast<UCanvasPanelSlot>(InPanelSlot);
    if (ensure(PanelSlot))
    {
        PanelSlot->SetPosition(FVector2D(OwningItem->TopLeftPos.X * GetSlotSize(), OwningItem->TopLeftPos.Y * GetSlotSize()));
        FVector2D Size = FVector2D(OwningItem->TileSize.X * GetSlotSize(), OwningItem->TileSize.Y * GetSlotSize());
        PanelSlot->SetSize(Size);
    }
}

void UGridBasedListEntry::NativeConstruct()
{
    Super::NativeConstruct();
}

void UGridBasedListEntry::SynchronizeProperties()
{
    Super::SynchronizeProperties();


}