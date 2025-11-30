// Fill out your copyright notice in the Description page of Project Settings.


#include "GridBasedListView.h"
#include "DragPayload.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "CommonHierarchicalScrollBox.h"

#include "Components/SizeBox.h"
#include "Components/Border.h"

#include "GridBasedObjectListEntry.h"

#include "HAL/IConsoleManager.h"

#include "Components/ListView.h"

void UGridBasedListView::NativeOnStartDetectDrag(UUserWidget* InDraggingWidget, const FPointerEvent& InEvent)
{
	IDragDetectable::NativeOnStartDetectDrag(InDraggingWidget, InEvent);

	if (nullptr == InventoryPanel || nullptr == FootprintWidget) { return; }

	UCanvasPanelSlot* FootprintWidgetSlot = Cast<UCanvasPanelSlot>(FootprintWidget->Slot);
	if (nullptr == FootprintWidgetSlot) return;

	UGridBasedListEntry* ListEntry = Cast<UGridBasedListEntry>(InDraggingWidget);
	if (nullptr == ListEntry) { return; }

	const UGridBasedListItem* ListItem = GetItemFromListEntry(ListEntry);
	if (nullptr == ListItem) { return; }

	// Set Footprint
	TArray<int> HiddenIdx;

	Algo::Transform(ListItem->HiddenIndexes, HiddenIdx, [&](const FIntPoint& InPoint) -> int32
		{
			return MakeKey(InPoint.Y, InPoint.X);
		});

	FootprintWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	FootprintWidget->SetFootprint(ListItem->TileSize.X, ListItem->TileSize.Y, HiddenIdx);

	FootprintWidgetSlot->SetSize(PointToLocal(ListItem->TileSize));

	const FVector2D CursorScreenSpacePos = InEvent.GetScreenSpacePosition();
	const FVector2D InventorySpacePos = InventoryPanel->GetTickSpaceGeometry().AbsoluteToLocal(CursorScreenSpacePos);

	ForEach(ListItem->TopLeftPos.X, ListItem->TopLeftPos.Y, ListItem->TileSize, [&](int32 InIndex)
		{
			if (Grid.IsValidIndex(InIndex))
			{
				Grid[InIndex] = nullptr;
			}
		});

	FIntPoint TopLeftPoint = LocalToPoint(InventorySpacePos);
	TopLeftPoint.X = FMath::Clamp(TopLeftPoint.X, 0, RowCount - ListItem->TileSize.X);
	TopLeftPoint.Y = FMath::Clamp(TopLeftPoint.Y, 0, ColumnCount - ListItem->TileSize.Y);

	bool bIsEmpty = true == IsEmptySpace(TopLeftPoint, ListItem->TileSize);

	if (true == bIsEmpty)
	{
		FootprintWidget->SetStyle(FootprintStyles[EStorageState::Empty]);
	}

	else
	{
		FootprintWidget->SetStyle(FootprintStyles[EStorageState::IsFull]);
	}

	FootprintWidgetSlot->SetPosition(PointToLocal(TopLeftPoint));
	TempFootprintLoc = TopLeftPoint;
}
void UGridBasedListView::NativeOnEndDetect(UUserWidget* InDraggingWidget, const FPointerEvent& InEvent)
{
	IDragDetectable::NativeOnEndDetect(InDraggingWidget, InEvent);

	if (nullptr != FootprintWidget)
	{
		FootprintWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}
void UGridBasedListView::NativeOnDetect(UUserWidget* InDraggingWidget, const FPointerEvent& InEvent)
{
	IDragDetectable::NativeOnDetect(InDraggingWidget, InEvent);

	UCanvasPanelSlot* FootprintWidgetSlot = Cast<UCanvasPanelSlot>(FootprintWidget->Slot);
	if (nullptr == FootprintWidgetSlot) { return; };

	UGridBasedListEntry* ListEntry = Cast<UGridBasedListEntry>(InDraggingWidget);
	if (nullptr == ListEntry) { return; }

	const UGridBasedListItem* ListItem = GetItemFromListEntry(ListEntry);
	if (nullptr == ListItem) { return; }

	if (nullptr == InventoryPanel || nullptr == FootprintWidget) { return; }
	const FVector2D CursorScreenSpacePos = InEvent.GetScreenSpacePosition();
	const FVector2D InventorySpacePos = InventoryPanel->GetTickSpaceGeometry().AbsoluteToLocal(CursorScreenSpacePos);

	FIntPoint TopLeftPoint = LocalToPoint(InventorySpacePos);
	TopLeftPoint.X = FMath::Clamp(TopLeftPoint.X, 0, RowCount - ListItem->TileSize.X);
	TopLeftPoint.Y = FMath::Clamp(TopLeftPoint.Y, 0, ColumnCount - ListItem->TileSize.Y);

	bool bIsEmpty = true == IsEmptySpace(TopLeftPoint, ListItem->TileSize);

	if (true == bIsEmpty)
	{
		FootprintWidget->SetStyle(FootprintStyles[EStorageState::Empty]);
	}

	else
	{
		FootprintWidget->SetStyle(FootprintStyles[EStorageState::IsFull]);
	}

	FootprintWidgetSlot->SetPosition(PointToLocal(TopLeftPoint));
	TempFootprintLoc = TopLeftPoint;
}
void UGridBasedListView::NativeOnDrop(UUserWidget* InDraggingWidget, const FPointerEvent& InEvent)
{
	IDragDetectable::NativeOnDrop(InDraggingWidget, InEvent);

	if (nullptr != FootprintWidget)
	{
		FootprintWidget->SetVisibility(ESlateVisibility::Collapsed);
	}

	UGridBasedListEntry* ListEntry = Cast<UGridBasedListEntry>(InDraggingWidget);
	if (nullptr == ListEntry) { return; }

	if (TempFootprintLoc.X == -1 && TempFootprintLoc.Y == -1)
	{
		return;
	}

	UGridBasedListItem* const* FoundItem = ListItems.FindByPredicate([&](UGridBasedListItem* InItem)
		{
			return InItem == GetItemFromListEntry(ListEntry);
		});

	if (nullptr == FoundItem || nullptr == *FoundItem) { return; }
	UGridBasedListItem* ListItem = *FoundItem;

	bool bIsEmpty = IsEmptySpace(TempFootprintLoc, ListItem->TileSize);

	FIntPoint DropPos = true == bIsEmpty ? TempFootprintLoc : ListItem->TopLeftPos;
	ForEach(DropPos.X, DropPos.Y, ListItem->TileSize, [&](int32 InIndex)
		{
			if (Grid.IsValidIndex(InIndex))
			{
				Grid[InIndex] = ListItem;
			}
		});

	ListItem->TopLeftPos = FIntPoint(DropPos.X, DropPos.Y);

	UCanvasPanelSlot* PanelSlot = InventoryPanel->AddChildToCanvas(ListEntry);
	if (ensure(PanelSlot))
	{
		PanelSlot->SetPosition(PointToLocal(DropPos));
		FVector2D Size = FVector2D(ListItem->TileSize.X * SlotSize, ListItem->TileSize.Y * SlotSize);
		PanelSlot->SetSize(Size);
	}

	TempFootprintLoc = FIntPoint(-1, -1);

}


void UGridBasedListView::SetListItems(const TArray<UGridBasedListItem*>& InItemList)
{


	ClearListItems();
	ListItems.Reserve(InItemList.Num());

	for (UGridBasedListItem* ListItem : InItemList)
	{
		if (nullptr == ListItem)
		{
			continue;
		}

		if (false == IsEmptySpace(ListItem->TopLeftPos, ListItem->TileSize))
		{
			UE_LOG(LogTemp, Error, TEXT("Not enough empty space.."));
			continue;
		}

		TArray<int32> Indexes;
		if (true == GetIndexes(OUT Indexes, ListItem->TopLeftPos, ListItem->TileSize))
		{
			for (int32 Index : Indexes)
			{
				Grid[Index] = ListItem;
			}

			ListItems.Add(ListItem);
		}
	}

	Refresh();
}

void UGridBasedListView::ClearListItems()
{
	ListItems.Reset();
	Refresh();
}


void UGridBasedListView::Refresh()
{
	if (nullptr != InventoryPanel)
	{
		for (auto& ChildWidget : InventoryPanel->GetAllChildren())
		{
			UGridBasedListEntry* ListEntry = Cast<UGridBasedListEntry>(ChildWidget);
			if (nullptr == ListEntry) continue;

			InventoryPanel->RemoveChild(ListEntry);
			ItemPool.Release(ListEntry);
		}
	}

	for (UGridBasedListItem* ListItem : ListItems)
	{
		if (false == IsOverScroll(
			MakeKey(ListItem->TopLeftPos.X, ListItem->TopLeftPos.Y)))
		{
			AddWidget(ListItem);
		}
	}
}

void UGridBasedListView::AddItem(UGridBasedListItem* InItem)
{
	if (!ensure(InItem))
	{
		return;
	}
	
	TArray<int32> GridList;
	int32 PossibleTopLeftKey = GetEmptyTopLeftKey(OUT GridList, InItem->TileSize);
	if (INDEX_NONE == PossibleTopLeftKey)
	{
		UE_LOG(LogTemp, Error, TEXT("Not enough empty space.."));
		return;
	}

	ListItems.Add(InItem);

	int32 TopLeftX = PossibleTopLeftKey % RowCount;
	int32 TopLeftY = PossibleTopLeftKey / RowCount;

	for (int32 Index : GridList)
	{
		Grid[Index] = InItem;
	}

	InItem->TopLeftPos = FIntPoint(TopLeftX, TopLeftY);

	if (false == IsOverScroll(PossibleTopLeftKey))
	{
		AddWidget(InItem);
	}
}

void UGridBasedListView::RemoveItem(UGridBasedListItem* InItem)
{
	if (ActiveWidgets.Contains(InItem))
	{
		ForEach(InItem->TopLeftPos.X, InItem->TopLeftPos.Y, InItem->TileSize, [&](int32 InIndex)
			{
				if (Grid.IsValidIndex(InIndex))
				{
					Grid[InIndex] = nullptr;
				}
			});

		ListItems.Remove(InItem);

		if (false == IsOverScroll(MakeKey(InItem->TopLeftPos.X, InItem->TopLeftPos.Y)))
		{
			RemoveWidget(InItem);
		}

		if (SelectedItems.Contains(InItem))
		{
			SelectedItems.Remove(InItem);
		}

		if (DeActivedSelectedItems.Contains(InItem))
		{
			DeActivedSelectedItems.Remove(InItem);
		}
	}

}

const UUserWidget* UGridBasedListView::GetEntry(UGridBasedListItem* InItem) const
{
	if (nullptr != InItem && ActiveWidgets.Contains(InItem))
	{
		TWeakObjectPtr<UUserWidget> FoundWidget = ActiveWidgets.FindRef(InItem);
		if (FoundWidget.IsValid())
		{
			return FoundWidget.Get();
		}
	}

	return nullptr;
}

const UGridBasedListItem* UGridBasedListView::GetItemFromListEntry(UUserWidget* InWidget) const
{
	if (nullptr != InWidget && ActiveItems.Contains(InWidget))
	{
		return ActiveItems.FindRef(InWidget);
	}

	return nullptr;
}

void UGridBasedListView::OnChangedScrollOffset(float InScrollOffset)
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

			const UGridBasedListItem* Item = Grid[NewKey];

			if (nullptr != Item)
			{
				int ItemBottomYIndex = Item->TopLeftPos.Y + (Item->TileSize.Y - 1);
				if (ItemBottomYIndex <= RemovePage && ActiveWidgets.Contains(Item))
				{
					RemoveWidget(Item);
				}
			}
		}

		// add item widget
		for (int i = 0; i < RowCount; ++i)
		{
			int NewKey = MakeKey(i, NewPage);
			if (!Grid.IsValidIndex(NewKey)) { continue; }

			UGridBasedListItem* const* FindItem = ListItems.FindByPredicate([&](UGridBasedListItem* InItem)
				{
					const UGridBasedListItem* Item = Grid[NewKey];

					return nullptr != Item && InItem == Item;
				});

			if (nullptr != FindItem && nullptr != *FindItem && false == ActiveWidgets.Contains(*FindItem))
			{
				AddWidget(*FindItem);
			}
		}

		CurrentPage = NewCurrentPageIndex;
	}
	
	if (GEngine && InventoryPanel)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("Current Height = %d, Active Widget Num = %d"), NewCurrentPageIndex, InventoryPanel->GetChildrenCount()));
	}
}

void UGridBasedListView::SetVisibleScrollBar(bool bIsVisible)
{
	if (nullptr != ScrollBox)
	{
		ScrollBox->SetScrollBarVisibility(true == bIsVisible ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
	}
}

void UGridBasedListView::NativeConstruct()
{
	Super::NativeConstruct();

	if (ensureAlways(GetOwningPlayer()))
	{
		ItemPool.SetWorld(GetOwningPlayer()->GetWorld());
		ItemPool.ReleaseAll();
	}

	if (nullptr != ScrollBox)
	{
		ScrollBox->OnUserScrolled.AddDynamic(this, &UGridBasedListView::OnChangedScrollOffset);
		ScrollBox->SetScrollOffset(0);
		CurrentPage = 0;
	}

	SetInventorySize();
	SetMaterial();

	if (nullptr != FootprintWidget)
	{
		FootprintWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UGridBasedListView::NativeDestruct()
{
	ItemPool.ReleaseAll(true);
	ItemPool.ResetPool();

	Grid.Reset();
	ListItems.Reset();
	ActiveWidgets.Reset();
	ActiveItems.Reset();
	HiddenIndex.Reset();
	SelectedItems.Reset();
	DeActivedSelectedItems.Reset();

	Super::NativeDestruct();
}

void UGridBasedListView::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	SetInventorySize();
	SetMaterial();

}

void UGridBasedListView::AddWidget(UGridBasedListItem* InItem)
{
	UUserWidget* ActivedWidget = ItemPool.GetOrCreateInstance(ItemWidgetClass);
	if (ensure(nullptr != ActivedWidget))
	{
		if (IGridBasedObjectListEntry* InterfaceEntry = Cast<IGridBasedObjectListEntry>(ActivedWidget))
		{
			InterfaceEntry->OwningListView = this;
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
				PanelSlot->SetPosition(PointToLocal(InItem->TopLeftPos));

				FVector2D Size = FVector2D(InItem->TileSize.X * SlotSize, InItem->TileSize.Y * SlotSize);
				PanelSlot->SetSize(Size);
			}
		}

		ActiveWidgets.Emplace(InItem, ActivedWidget);
		ActiveItems.Emplace(ActivedWidget, InItem);

		if (UGridBasedListEntry* ListEntry = Cast<UGridBasedListEntry>(ActivedWidget))
		{
			OnEntryGenerated.ExecuteIfBound(*ListEntry);
		}
	}
}

void UGridBasedListView::RemoveWidget(const UGridBasedListItem* InItem)
{
	if (false == ActiveWidgets.Contains(InItem))
	{
		return;
	}

	TWeakObjectPtr<UUserWidget> ActivedWidget = ActiveWidgets[InItem];
	if (ensure(ActivedWidget.IsValid()))
	{
		if (UGridBasedListEntry* ListEntry = Cast<UGridBasedListEntry>(ActivedWidget))
		{
			OnEntryReleased.ExecuteIfBound(*ListEntry);
		}

		if (IGridBasedObjectListEntry* InterfaceEntry = Cast<IGridBasedObjectListEntry>(ActivedWidget.Get()))
		{
			InterfaceEntry->NativeOnEntryReleased();
		}

		else if (ActivedWidget.Get()->GetClass() && ActivedWidget.Get()->GetClass()->ImplementsInterface(UGridBasedObjectListEntry::StaticClass()))
		{
			IGridBasedObjectListEntry::Execute_OnEntryReleased(ActivedWidget.Get());
		}

		if (InventoryPanel)
		{
			InventoryPanel->RemoveChild(ActivedWidget.Get());
		}

		ItemPool.Release(ActivedWidget.Get());
		ActiveItems.Remove(ActivedWidget.Get());

		ActiveWidgets.Remove(InItem);
	}
}

int32 UGridBasedListView::MakeKey(int32 InRow, int32 InColumn)
{
	return InRow + (InColumn * RowCount);
}

FVector2D UGridBasedListView::PointToLocal(const FIntPoint& InPoint)
{
	return FVector2D(InPoint.X * SlotSize, InPoint.Y * SlotSize);
}

FIntPoint UGridBasedListView::LocalToPoint(const FVector2D& InLocalVec)
{
	return FIntPoint(InLocalVec.X / SlotSize, InLocalVec.Y / SlotSize);
}

int32 UGridBasedListView::GetEmptyTopLeftKey(OUT TArray<int32>& OutGridList, const FIntPoint& InItemSize)
{
	TQueue<int32> TopLeftQueue;

	for (int i = 0; i < Grid.Num(); ++i)
	{
		if (nullptr == Grid[i])
		{
			TopLeftQueue.Enqueue(i);
		}
	}

	OutGridList.Reserve(InItemSize.X * InItemSize.Y);

	while (false == TopLeftQueue.IsEmpty())
	{
		int32 TopLeft;
		TopLeftQueue.Dequeue(OUT TopLeft);

		if (true == IsEmptySpace(TopLeft, InItemSize))
		{
			if (ensure(true == GetIndexes(OUT OutGridList, TopLeft, InItemSize)))
			{
				return OutGridList.IsValidIndex(0) ? OutGridList[0] : INDEX_NONE;
			}
		}
	}

	return INDEX_NONE;
}

void UGridBasedListView::SetMaterial()
{
	if (nullptr != InventoryBG)
	{
		if (UMaterialInstanceDynamic* Material = InventoryBG->GetDynamicMaterial())
		{
			Material->SetScalarParameterValue(TEXT("TileX"), RowCount);
			Material->SetScalarParameterValue(TEXT("TileY"), ColumnCount);
			Material->SetTextureParameterValue(TEXT("Pattern"), SlotTexture);

			InventoryBG->SetBrushColor(SlotColor);

			TArray<int> Index;
			for (const FIntPoint& Indexes : HiddenIndex)
			{
				int X = FMath::Clamp(Indexes.X, 0, RowCount);
				int Y = FMath::Clamp(Indexes.Y, 0, ColumnCount);

				Index.Emplace(X + RowCount * Y);
			}

			uint32 W0 = 0, W1 = 0, W2 = 0, W3 = 0;

			for (int32 i : Index)
			{
				if (i < 0 || i >= 128) continue;
				const uint32 word = (uint32)i >> 5;    // 0..3 (�� 32ĭ)
				const uint32 bit = (uint32)i & 31u;   // 0..31
				const uint32 m = 1u << bit;

				switch (word)
				{
				case 0: W0 |= m; break; // 0..31   -> R
				case 1: W1 |= m; break; // 32..63  -> G
				case 2: W2 |= m; break; // 64..95  -> B
				case 3: W3 |= m; break; // 96..127 -> A
				default: break;
				}
			}

			auto PackToFColor = [](uint32 a, uint32 b, uint32 c, uint32 d) -> FLinearColor
				{
					return FLinearColor(
						*reinterpret_cast<float*>(&a),
						*reinterpret_cast<float*>(&b),
						*reinterpret_cast<float*>(&c),
						*reinterpret_cast<float*>(&d));
				};

			Material->SetVectorParameterValue(TEXT("ExcludedBitsA"), PackToFColor(W0, W1, W2, W3));
			Material->SetScalarParameterValue(TEXT("TileX"), RowCount);
			Material->SetScalarParameterValue(TEXT("TileY"), ColumnCount);
			Material->SetVectorParameterValue(TEXT("ExcludedBitsB"), FLinearColor::Black);
		}
	}
}

void UGridBasedListView::SetInventorySize()
{
	if (nullptr != InventorySizeBox)
	{
		InventorySizeBox->SetWidthOverride(SlotSize * RowCount);
		InventorySizeBox->SetHeightOverride(SlotSize * ColumnCount);
	}

	Grid.SetNumZeroed(RowCount * ColumnCount);
}

bool UGridBasedListView::IsOverScroll(int32 InTopLeftKey) const
{
	if (!ensure(ScrollBox && InventorySizeBox)) { return false; }

	const float CurrentScrollOffset = ScrollBox->GetScrollOffset();
	const float InventorySize = InventorySizeBox->GetHeightOverride();

	const int32 LastPageIndex = FMath::CeilToInt32((CurrentScrollOffset + InventorySize) / SlotSize);
	const int32 FirstPageIndex = LastPageIndex - ColumnCount;
	
	const int32 CurrentPageIndex = InTopLeftKey / RowCount;

	return FirstPageIndex > CurrentPageIndex;
}

bool UGridBasedListView::IsEmptySpace(const FIntPoint& InTopLeftPoint, const FIntPoint& InSize)
{
	for (int32 i = 0; i < InSize.Y; ++i)
	{
		if (InTopLeftPoint.Y + i >= ColumnCount)
		{
			return false;
		}

		for (int32 j = 0; j < InSize.X; ++j)
		{
			if (InTopLeftPoint.X + j >= RowCount)
			{
				return false;
			}

			int32 Index = MakeKey(InTopLeftPoint.X + j, InTopLeftPoint.Y + i);

			if (Grid.IsValidIndex(Index) &&
				nullptr != Grid[Index])
			{
				return false;
			}
		}
	}


	return true;
}

bool UGridBasedListView::GetIndexes(OUT TArray<int32>& OutIndexList, const FIntPoint& InTopLeft, const FIntPoint& InSize)
{
	OutIndexList.Reset();

	for (int i = 0; i < InSize.Y; ++i)
	{
		for (int j = 0; j < InSize.X; ++j)
		{
			OutIndexList.Emplace(MakeKey(InTopLeft.X + j, InTopLeft.Y + i));
		}
	}

	return OutIndexList.Num() == InSize.X * InSize.Y;
}

void UGridBasedListView::ForEach(int32 InX, int32 InY, const FIntPoint& InSize, TFunctionRef<void(int32)> InFunc)
{
	for (int i = 0; i < InSize.Y; ++i)
	{
		for (int j = 0; j < InSize.X; ++j)
		{
			int32 NewIndex = MakeKey(InX + j, InY + i);
			InFunc(NewIndex);
		}
	}
}

void UGridBasedListView::SetSelect(const UGridBasedListItem* InItem)
{
	UUserWidget* ListEntry = ActiveWidgets.FindRef(InItem);
	UGridBasedListItem* ListItem = ActiveItems.FindRef(ListEntry);

	if (true == ActiveWidgets.Contains(InItem))
	{
		if (true == SelectedItems.Contains(InItem))
		{
			SelectedItems.Remove(InItem);
			OnSelectionChanged.ExecuteIfBound(ListItem, false);
		}

		/*else if (true == DeActivedSelectedItems.Contains(InItem))
		{
			DeActivedSelectedItems.Remove(InItem);
		}*/

		else
		{
			SelectedItems.Emplace(InItem);
			OnSelectionChanged.ExecuteIfBound(ListItem, true);
		}

	}

	else
	{
		if (true == DeActivedSelectedItems.Contains(InItem))
		{
			DeActivedSelectedItems.Remove(InItem);
		}
	}
}

bool UGridBasedListView::IsSelectedItem(const UGridBasedListItem* InItem) const
{
	return true == SelectedItems.Contains(InItem);
}

float UGridBasedListView::GetSlotSize() const
{
	return SlotSize;
}