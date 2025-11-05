// Fill out your copyright notice in the Description page of Project Settings.


#include "GridBasedInventoryList.h"
#include "GridBasedInventoryEntry.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "CommonHierarchicalScrollBox.h"

#include "Components/SizeBox.h"
#include "Components/Border.h"

#include "GridBasedObjectListEntry.h"

#include "HAL/IConsoleManager.h"

void UGridBasedInventoryList::AddItem(UGridBasedInventoryItem* InItem)
{
	if (!ensure(InItem))
	{
		return;
	}
	
	TArray<int32> GridList;
	int32 PossibleTopLeftKey = GetBlankedSpaceIndex(OUT GridList, InItem->Size.X, InItem->Size.Y);
	if (INDEX_NONE == PossibleTopLeftKey)
	{
		UE_LOG(LogTemp, Error, TEXT("Not enough empty space.."));
		return;
	}

	int32 TopLeftX = PossibleTopLeftKey % RowCount;
	int32 TopLeftY = PossibleTopLeftKey / RowCount;

	for (int32 Index : GridList)
	{
		Grid[Index] = InItem;
	}

	InItem->TopLeft = FVector2D(TopLeftX, TopLeftY);

	if (false == IsOverScroll(PossibleTopLeftKey))
	{
		AddWidget(InItem);
	}
}

void UGridBasedInventoryList::RemoveItem(UGridBasedInventoryItem* InItem)
{

}

UUserWidget* UGridBasedInventoryList::GetEntry(UGridBasedInventoryItem* InItem)
{
	return nullptr;
}

UGridBasedInventoryItem* UGridBasedInventoryList::GetItemFromListEntry(UUserWidget* InWidget)
{
	return nullptr;
}


void UGridBasedInventoryList::OnChangedScrollOffset(float InScrollOffset)
{
	const float InventorySize = InventorySizeBox->GetHeightOverride();

	const int32 LastPageIndex = FMath::CeilToInt32((InScrollOffset + InventorySize) / SlotSize);
	const int32 FirstPageIndex = LastPageIndex - ColumnCount;

	const int32 NewCurrentPageIndex = InScrollOffset / SlotSize;

	if (CurrentPage != NewCurrentPageIndex)
	{
		int32 RemovePage = 0;
		int32 NewPage = 0;

		if (CurrentPage < NewCurrentPageIndex)
		{
			RemovePage = CurrentPage;
			NewPage = LastPageIndex;
		}

		else
		{
			RemovePage = LastPageIndex + (CurrentPage - NewCurrentPageIndex);
			NewPage = NewCurrentPageIndex;
		}

		// remove widget
		for (int i = 0; i < RowCount; ++i)
		{
			int NewKey = MakeKey(i, RemovePage);
			if (!Grid.IsValidIndex(NewKey)) { continue; }

			UGridBasedInventoryItem* Item = Grid[NewKey];
			if (nullptr != Item && Item->Size.Y <= (RemovePage + 1) && ActiveWidgets.Contains(Item))
			{
				RemoveWidget(Item);
			}
		}

		// add item widget
		for (int i = 0; i < RowCount; ++i)
		{
			int NewKey = MakeKey(i, NewPage);
			if (!Grid.IsValidIndex(NewKey)) { continue; }

			UGridBasedInventoryItem* Item = Grid[NewKey];
			if (nullptr != Item && false == ActiveWidgets.Contains(Item))
			{
				AddWidget(Item);
			}
		}

		CurrentPage = NewCurrentPageIndex;
	}
	
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("Current Height = %d, Active Widget Num = %d"), NewCurrentPageIndex, ActiveWidgets.Num()));
	}
}

void UGridBasedInventoryList::NativeConstruct()
{
	Super::NativeConstruct();

	if (ensureAlways(GetOwningPlayer()))
	{
		ItemPool.SetWorld(GetOwningPlayer()->GetWorld());
		ItemPool.ReleaseAll();
	}

	if (nullptr != ScrollBox)
	{
		ScrollBox->OnUserScrolled.AddDynamic(this, &UGridBasedInventoryList::OnChangedScrollOffset);
		ScrollBox->SetScrollOffset(0);
		CurrentPage = 0;
	}

	SetInventorySize();
	SetMaterial();
}

void UGridBasedInventoryList::NativeDestruct()
{
	ItemPool.ReleaseAll(true);
	ItemPool.ResetPool();

	Grid.Reset();


	Super::NativeDestruct();
}

void UGridBasedInventoryList::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	SetInventorySize();
	SetMaterial();	
}

void UGridBasedInventoryList::AddWidget(UGridBasedInventoryItem* InItem)
{
	UUserWidget* ActivedWidget = ItemPool.GetOrCreateInstance(ItemWidgetClass);
	if (ensure(nullptr != ActivedWidget))
	{
		if (IGridBasedObjectListEntry* InterfaceEntry = Cast<IGridBasedObjectListEntry>(ActivedWidget))
		{
			InterfaceEntry->NativeOnListItemObjectSet(InItem);
		}

		else if (ItemWidgetClass->ImplementsInterface(UGridBasedObjectListEntry::StaticClass()))
		{
			IGridBasedObjectListEntry::Execute_OnListItemObjectSet(ActivedWidget, InItem);
		}

		if (InventoryPanel)
		{
			UCanvasPanelSlot* PanelSlot = InventoryPanel->AddChildToCanvas(ActivedWidget);
			if (ensure(PanelSlot))
			{
				PanelSlot->SetPosition(ConvertCanvasPosition(InItem->TopLeft.X, InItem->TopLeft.Y));

				FVector2D Size = FVector2D(InItem->Size.X * SlotSize, InItem->Size.Y * SlotSize);
				PanelSlot->SetSize(Size);
			}
		}

		ActiveWidgets.Emplace(InItem, ActivedWidget);
	}
}

void UGridBasedInventoryList::RemoveWidget(UGridBasedInventoryItem* InItem)
{
	TWeakObjectPtr<UUserWidget> ActivedWidget = ActiveWidgets[InItem];
	if (ensure(ActivedWidget.IsValid()))
	{
		if (IGridBasedObjectListEntry* InterfaceEntry = Cast<IGridBasedObjectListEntry>(ActivedWidget.Get()))
		{
			InterfaceEntry->NativeOnEntryReleased();
		}

		else if (ActivedWidget.Get()->GetClass() && ActivedWidget.Get()->GetClass()->ImplementsInterface(UGridBasedObjectListEntry::StaticClass()))
		{
			IGridBasedObjectListEntry::Execute_OnEntryReleased(ActivedWidget.Get());
		}

		ItemPool.Release(ActivedWidget.Get());
		ActiveWidgets.Remove(InItem);
	}
}

int32 UGridBasedInventoryList::MakeKey(uint32 InRow, uint32 InColumn)
{
	return InRow + (InColumn * RowCount);
}

FVector2D UGridBasedInventoryList::ConvertCanvasPosition(uint32 InSlotW, uint32 InSlotH)
{
	return FVector2D(SlotSize * InSlotW, SlotSize * InSlotH);
}

int32 UGridBasedInventoryList::GetBlankedSpaceIndex(OUT TArray<int32>& OutGridList, uint8 InWidth, uint8 InHeight)
{
	TQueue<int32> Queue;

	for (int i = 0; i < Grid.Num(); ++i)
	{
		if (nullptr == Grid[i])
		{
			Queue.Enqueue(i);
		}
	}

	OutGridList.Reserve(InWidth * InHeight);

	while (false == Queue.IsEmpty())
	{
		int32 TopLeft;
		Queue.Dequeue(OUT TopLeft);

		int Level = TopLeft / RowCount;
		if (Level != (TopLeft + InWidth - 1) / RowCount)
		{
			continue;
		}

		OutGridList.Reset();

		bool flag = false;

		for (int i = 0; i < InHeight; ++i)
		{
			int Height = i * RowCount;

			for (int j = 0; j < InWidth; ++j)
			{
				int Index = TopLeft + j + Height;
				OutGridList.Emplace(Index);

				if (false == Grid.IsValidIndex(Index) ||
					nullptr != Grid[Index])
				{
					flag = true;
					break;
				}
			}

			if (true == flag)
			{
				break;
			}
		}

		if (OutGridList.Num() == InWidth * InHeight)
		{
			return OutGridList.IsValidIndex(0) ? OutGridList[0] : INDEX_NONE;
		}
	}

	return INDEX_NONE;
}

void UGridBasedInventoryList::SetMaterial()
{
	if (nullptr != InventoryBG)
	{
		if (UMaterialInstanceDynamic* Material = InventoryBG->GetDynamicMaterial())
		{
			Material->SetScalarParameterValue(TEXT("CellX"), RowCount);
			Material->SetScalarParameterValue(TEXT("CellY"), ColumnCount);
			Material->SetTextureParameterValue(TEXT("Texture"), SlotTexture);
		}
	}
}

void UGridBasedInventoryList::SetInventorySize()
{
	if (nullptr != InventorySizeBox)
	{
		InventorySizeBox->SetWidthOverride(SlotSize * RowCount);
		InventorySizeBox->SetHeightOverride(SlotSize * ColumnCount);
	}

	Grid.SetNumZeroed(RowCount * ColumnCount);
}

bool UGridBasedInventoryList::IsOverScroll(int32 InTopLeftKey) const
{
	if (!ensure(ScrollBox && InventorySizeBox)) { return false; }

	const float CurrentScrollOffset = ScrollBox->GetScrollOffset();
	const float InventorySize = InventorySizeBox->GetHeightOverride();

	const int32 LastPageIndex = FMath::CeilToInt32((CurrentScrollOffset + InventorySize) / SlotSize);
	const int32 FirstPageIndex = LastPageIndex - ColumnCount;
	
	const int32 CurrentPageIndex = InTopLeftKey / RowCount;

	return FirstPageIndex > CurrentPageIndex;
}
