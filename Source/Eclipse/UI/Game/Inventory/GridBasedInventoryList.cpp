// Fill out your copyright notice in the Description page of Project Settings.


#include "GridBasedInventoryList.h"
#include "GridBasedInventoryEntry.h"

#include "CommonHierarchicalScrollBox.h"

#include "Components/SizeBox.h"
#include "Components/GridPanel.h"
#include "Components/GridSlot.h"

void UGridBasedInventoryList::NativeConstruct()
{
	Super::NativeConstruct();

	if (ensureAlways(GetOwningPlayer()))
	{
		SlotPool.SetWorld(GetOwningPlayer()->GetWorld());
	}

	if (nullptr != ScrollBox)
	{
		ScrollBox->OnUserScrolled.AddDynamic(this, &UGridBasedInventoryList::OnChangedScrollOffset);
		ScrollBox->SetScrollOffset(CurrentScrollOffset = 0);
	}

	if (GetOwningPlayer())
	{
		SlotPool.SetWorld(GetOwningPlayer()->GetWorld());
		SlotPool.ReleaseAll();
	}

	if (nullptr != InventorySizeBox)
	{
		InventorySizeBox->SetWidthOverride(SlotWidth * SlotRow);
		InventorySizeBox->SetHeightOverride(SlotHeight * SlotColumn);
	}

	if (EntryClass && nullptr != GridPanel)
	{
		for (int i = 0; i < SlotWidth; ++i)
		{
			for (int j = 0; j < SlotColumn; ++j)
			{
				UGridBasedInventoryEntry* Entry = SlotPool.GetOrCreateInstance(EntryClass);
				if (!ensure(Entry)) continue;

				Entry->SetSize(FVector2D(SlotWidth, SlotHeight));

				if (UGridSlot* GridSlot = GridPanel->AddChildToGrid(Entry))
				{
					GridSlot->SetRow(i);
					GridSlot->SetColumn(j);
				}
			}
		}
	}

}

void UGridBasedInventoryList::NativeDestruct()
{
	SlotPool.ReleaseAll(true);
	SlotPool.ResetPool();

	Super::NativeDestruct();
}

void UGridBasedInventoryList::OnChangedScrollOffset(float InScrollOffset)
{

}

void UGridBasedInventoryList::DebugInventoryList()
{
	if (IsDesignTime())
	{
		if (nullptr != InventorySizeBox)
		{
			InventorySizeBox->SetWidthOverride(SlotWidth * SlotRow);
			InventorySizeBox->SetHeightOverride(SlotHeight * SlotColumn);
		}

		if (EntryClass && nullptr != GridPanel)
		{
			GridPanel->ClearChildren();

			for (int i = 0; i < SlotWidth; ++i)
			{
				for (int j = 0; j < SlotColumn; ++j)
				{
					UGridBasedInventoryEntry* Entry = CreateWidget<UGridBasedInventoryEntry>(this, EntryClass);
					if (!ensure(Entry)) continue;

					Entry->SetSize(FVector2D(SlotWidth, SlotHeight));

					if (UGridSlot* GridSlot = GridPanel->AddChildToGrid(Entry))
					{
						GridSlot->SetRow(j);
						GridSlot->SetColumn(i);
					}
				}
			}
		}
	}
}
