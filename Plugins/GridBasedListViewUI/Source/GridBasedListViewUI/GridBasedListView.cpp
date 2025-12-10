// Fill out your copyright notice in the Description page of Project Settings.


#include "GridBasedListView.h"
#include "GridBasedObjectListEntry.h"
#include "DragPayload.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "CommonHierarchicalScrollBox.h"

#include "Components/SizeBox.h"
#include "Components/Border.h"
#include "Components/ListView.h"

#include "HAL/IConsoleManager.h"


void UGridBasedListView::NativeOnStartDetectDrag(UUserWidget* InDraggingWidget, const FPointerEvent& InEvent)
{
	// DraggableWidget : GridBasedListEntry

	IDragDetectable::NativeOnStartDetectDrag(InDraggingWidget, InEvent);

	if (nullptr == InventoryPanel || nullptr == FootprintWidget) { return; }



	UGridBasedListEntry* ListEntry = Cast<UGridBasedListEntry>(InDraggingWidget);
	if (nullptr == ListEntry) { return; }

	const UGridBasedListItem* ListItem = GetItemFromListEntry(ListEntry);
	if (nullptr == ListItem) { return; }

	// Set Footprint First
	TArray<int> HiddenIdx;
	Algo::Transform(ListItem->HiddenIndexes, HiddenIdx, [&](const FIntPoint& InPoint) -> int32
		{
			return MakeKey(InPoint.Y, InPoint.X);
		});

	FootprintWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	FootprintWidget->SetFootprint(ListItem->TileSize.X, ListItem->TileSize.Y, HiddenIdx);

	UCanvasPanelSlot* FootprintWidgetSlot = Cast<UCanvasPanelSlot>(FootprintWidget->Slot);
	if (nullptr != FootprintWidgetSlot)
	{
		FootprintWidgetSlot->SetSize(PointToLocal(ListItem->TileSize));
	}

	// 터치한 아이템 그리드에서 비워주기
	ForEach(ListItem->TopLeftPos.X, ListItem->TopLeftPos.Y, ListItem->TileSize, [&](int32 InIndex)
		{
			if (Grid.IsValidIndex(InIndex))
			{
				Grid[InIndex] = nullptr;
			}
		});

	SetPreviewDragFootprint(ListItem, InEvent.GetScreenSpacePosition());
}

void UGridBasedListView::NativeOnEndDetect(UUserWidget* InDraggingWidget, const FPointerEvent& InEvent)
{
	IDragDetectable::NativeOnEndDetect(InDraggingWidget, InEvent);

	if (nullptr != FootprintWidget)
	{
		FootprintWidget->SetVisibility(ESlateVisibility::Collapsed);
		TempFootprintLoc = { -1,-1 };
	}
}

void UGridBasedListView::NativeOnDetect(UUserWidget* InDraggingWidget, const FPointerEvent& InEvent)
{
	IDragDetectable::NativeOnDetect(InDraggingWidget, InEvent);

	if (nullptr == InventoryPanel || nullptr == FootprintWidget) { return; }

	UGridBasedListEntry* ListEntry = Cast<UGridBasedListEntry>(InDraggingWidget);
	if (nullptr == ListEntry) { return; }

	const UGridBasedListItem* ListItem = GetItemFromListEntry(ListEntry);
	if (nullptr == ListItem) { return; }

	SetPreviewDragFootprint(ListItem, InEvent.GetScreenSpacePosition());
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

	UGridBasedListItem* const* FoundItem = ListItems.FindByPredicate([&](UGridBasedListItem* InItem)
		{
			return InItem == GetItemFromListEntry(ListEntry);
		});

	if (nullptr == FoundItem || nullptr == *FoundItem) { return; }
	UGridBasedListItem* ListItem = *FoundItem;

	if (TempFootprintLoc.X == -1 && TempFootprintLoc.Y == -1)
	{
		Drop(ListItem);
		return;
	}

	bool bIsEmpty = IsEmptySpace(TempFootprintLoc, ListItem->TileSize);
	if (false == bIsEmpty)
	{
		UGridBasedListEntry* SwappableEntry = nullptr;
		UGridBasedListItem* PlacedItem = GetItemInPlace(TempFootprintLoc, ListItem->TileSize);

		if (OnGetCanPlace.IsBound() && true == OnGetCanPlace.Execute(ListItem, PlacedItem))
		{
			OnPlacedItem.Broadcast(ListItem, PlacedItem);
		}

		else if (IsPossibleSwap(OUT &SwappableEntry, TempFootprintLoc, ListItem))
		{
			Swap(ListEntry, SwappableEntry, TempFootprintLoc);
		}

		else
		{
			SetItemPosition(ListItem, ListItem->TopLeftPos);
		}

	}

	else
	{
		SetItemPosition(ListItem, TempFootprintLoc);
	}


	// 제자리로 이동
	/*const FIntPoint DropPos = TempFootprintLoc;
	ForEach(DropPos.X, DropPos.Y, ListItem->TileSize, [&](int32 InIndex)
		{
			if (Grid.IsValidIndex(InIndex))
			{
				Grid[InIndex] = ListItem;
			}
		});

	ListItem->TopLeftPos = DropPos;

	UCanvasPanelSlot* PanelSlot = InventoryPanel->AddChildToCanvas(ListEntry);
	if (ensure(PanelSlot))
	{
		PanelSlot->SetPosition(PointToLocal(DropPos));
		FVector2D Size = FVector2D(ListItem->TileSize.X * SlotSize, ListItem->TileSize.Y * SlotSize);
		PanelSlot->SetSize(Size);
	}*/

	TempFootprintLoc = FIntPoint(-1, -1);
}

FReply UGridBasedListView::NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnPreviewMouseButtonDown(InGeometry, InMouseEvent);

	if (nullptr == InventoryPanel)
	{
		return FReply::Handled();
	}

	const FVector2D CursorScreenSpacePos = InMouseEvent.GetScreenSpacePosition();
	const FVector2D InventorySpacePos = InventoryPanel->GetTickSpaceGeometry().AbsoluteToLocal(CursorScreenSpacePos);

	const FIntPoint TopLeftPoint = LocalToPoint(InventorySpacePos);
	const int32 IndexKey = MakeKey(TopLeftPoint.X, TopLeftPoint.Y);


	UE_LOG(LogTemp, Warning, TEXT("Index : %d, TopLeft : %d, %d"), IndexKey, TopLeftPoint.X, TopLeftPoint.Y);

	if (false == Grid.IsValidIndex(IndexKey))
	{
		return FReply::Handled();
	}

	const UGridBasedListItem* ListItem = Grid[IndexKey];

	if (false == ActiveWidgets.Contains(ListItem))
	{
		return FReply::Handled();
	}

	UUserWidget* ListEntry = ActiveWidgets[ListItem];
	SetMoveWidget(ListEntry);

	return FReply::Unhandled();
}

void UGridBasedListView::NativeOnDragCancel(UPanelSlot* InSlot)
{
	IDraggable::NativeOnDragCancel(InSlot);

	UGridBasedListEntry* MovedListEntry = Cast<UGridBasedListEntry>(InSlot->GetContent());

	if (!ensure(MovedListEntry))
	{
		return;
	}

	if (!ensure(ActiveItems.Contains(MovedListEntry)))
	{
		return;
	}

	UGridBasedListItem* ListItem = ActiveItems.FindRef(MovedListEntry);

	Drop(ListItem);
	TempFootprintLoc = FIntPoint(-1, -1);
}

void UGridBasedListView::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	OutOperation = CreateDragPayload([this](UDragDropOperation* InOperation)
		{
			if (UDragPayload* Payload = Cast<UDragPayload>(InOperation))
			{
				Payload->Payload = this;
				Payload->Pivot = DragPivot;
				Payload->Offset = DragOffset;
			}
		});
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
	
	// TODO : 데이터와 연동해서 조건이 맞는지 체크를 해야 함.
	// 조건 체크 로직 분리가 필요

	TArray<int32> GridList;
	int32 PossibleTopLeftKey = GetEmptyTopLeftKey(OUT GridList, InItem->TileSize);
	if (INDEX_NONE == PossibleTopLeftKey)
	{
		UE_LOG(LogTemp, Error, TEXT("Not enough empty space.."));
		OnFailedAddItem.Broadcast(InItem);

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

	SetWorld(GetWorld());
	SetEventFromBorder(InventoryBG);
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
			OnEntryGenerated.Broadcast(*ListEntry);
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
			OnEntryReleased.Broadcast(*ListEntry);
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

int32 UGridBasedListView::MakeKey(int32 InRow, int32 InColumn) const
{
	return InRow + (InColumn * RowCount);
}

FVector2D UGridBasedListView::PointToLocal(const FIntPoint& InPoint) const
{
	return FVector2D(InPoint.X * SlotSize, InPoint.Y * SlotSize);
}

FIntPoint UGridBasedListView::LocalToPoint(const FVector2D& InLocalVec) const
{
	return FIntPoint(InLocalVec.X / SlotSize, InLocalVec.Y / SlotSize);
}

FIntPoint UGridBasedListView::KeyToPoint(int32 InKey) const
{
	return FIntPoint(InKey % RowCount, InKey / RowCount);
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

		FIntPoint Point = KeyToPoint(TopLeft);

		if (true == IsEmptySpace(Point, InItemSize))
		{
			if (ensure(true == GetIndexes(OUT OutGridList, Point, InItemSize)))
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

			UPaperSprite* Sprite;
			Sprite;

			Material->SetTextureParameterValue(TEXT("Pattern"), SlotSprite);

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

bool UGridBasedListView::IsEmptySpace(const FIntPoint& InTopLeftPoint, const FIntPoint& InSize, const UGridBasedListItem* InExceptItem) const
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

			if (Grid.IsValidIndex(Index) && nullptr != Grid[Index] && InExceptItem != Grid[Index])
			{
				return false;
			}
		}
	}


	return true;
}

bool UGridBasedListView::GetIndexes(OUT TArray<int32>& OutIndexList, const FIntPoint& InTopLeft, const FIntPoint& InSize) const
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

UGridBasedListItem* UGridBasedListView::GetItemInPlace(const FIntPoint& InTopLeft, const FIntPoint& InSize) const
{
	TArray<int32> Indexes;
	TArray<const UGridBasedListItem*> ItemsInArea;
	GetIndexes(OUT Indexes, InTopLeft, InSize);

	Algo::Transform(Indexes, ItemsInArea, [&](int32 Index) -> const UGridBasedListItem*
		{
			return Grid.IsValidIndex(Index) ? Grid[Index] : nullptr;
		});

	ItemsInArea.RemoveAll([](const UGridBasedListItem* InItem) -> bool
		{
			return nullptr == InItem;
		});

	TSet<const UGridBasedListItem*> ItemInAreaSet(ItemsInArea);

	if (ItemInAreaSet.Num() == 1)
	{
		const UGridBasedListItem* Key = *ItemInAreaSet.begin();
		if (true == ActiveWidgets.Contains(Key))
		{
			UUserWidget* ListEntry = ActiveWidgets[Key];
			
			if (ActiveItems.Contains(ListEntry))
			{
				return ActiveItems[ListEntry];
			}
		}
	}

	return nullptr;
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

void UGridBasedListView::SetPreviewDragFootprint(const UGridBasedListItem* InItem, const FVector2D& InCursorScreenPos)
{
	const FVector2D Offset = GetClickOffset();
	const FVector2D CursorInventoryPos = InventoryPanel->GetTickSpaceGeometry().AbsoluteToLocal(InCursorScreenPos);

	FVector2D CalculatedCursorPos = CursorInventoryPos - Offset;
	FIntPoint TopLeftPoint = LocalToPoint(CalculatedCursorPos);

	TopLeftPoint.X = FMath::Clamp(TopLeftPoint.X, 0, RowCount - InItem->TileSize.X);
	TopLeftPoint.Y = FMath::Clamp(TopLeftPoint.Y, 0, ColumnCount - InItem->TileSize.Y);

	bool bIsEmpty = true == IsEmptySpace(TopLeftPoint, InItem->TileSize);

	if (true == bIsEmpty)
	{
		FootprintWidget->SetStyle(FootprintStyles[EStorageState::Empty]);
	}

	else
	{
		UGridBasedListEntry* SwappableEntry = nullptr;
		UGridBasedListItem* ItemInDragPlace = GetItemInPlace(TopLeftPoint, InItem->TileSize);

		if (OnGetCanPlace.IsBound() && true == OnGetCanPlace.Execute(InItem, ItemInDragPlace))
		{
			FootprintWidget->SetStyle(FootprintStyles[EStorageState::Empty]);
		}

		// 스왑 가능한지 체크?
		else if (true == IsPossibleSwap(&SwappableEntry, TopLeftPoint, InItem))
		{
			FootprintWidget->SetStyle(FootprintStyles[EStorageState::Empty]);
		}

		else
		{
			FootprintWidget->SetStyle(FootprintStyles[EStorageState::IsFull]);
		}
	}


	UCanvasPanelSlot* FootprintWidgetSlot = Cast<UCanvasPanelSlot>(FootprintWidget->Slot);
	if (nullptr != FootprintWidgetSlot) 
	{
		FootprintWidgetSlot->SetPosition(PointToLocal(TopLeftPoint));
	};

	TempFootprintLoc = TopLeftPoint;
}

bool UGridBasedListView::IsPossibleSwap(OUT UGridBasedListEntry** OutSwappableEntry, const FIntPoint& InTargetTopLeft, const UGridBasedListItem* InDraggedItem) const
{

	/*TArray<int32> Indexes;
	TArray<const UGridBasedListItem*> ItemsInArea;
	GetIndexes(OUT Indexes, InTargetTopLeft, InDraggedItem->TileSize);

	Algo::Transform(Indexes, ItemsInArea, [&](int32 Index) -> const UGridBasedListItem*
		{
			return Grid.IsValidIndex(Index) ? Grid[Index] : nullptr;
		});

	ItemsInArea.RemoveAll([](const UGridBasedListItem* InItem) -> bool
		{
			return nullptr == InItem;
		});

	TSet<const UGridBasedListItem*> ItemInAreaSet(ItemsInArea);

	check(ItemInAreaSet.Num() != 0);*/

	// 드래그 할 영역에 아이템이 1개 이하로 있을 때는 스왑을 시도한다.
	const UGridBasedListItem* DetectListItem = GetItemInPlace(InTargetTopLeft, InDraggedItem->TileSize);

	if (nullptr != DetectListItem)
	{
		UGridBasedListEntry* DetectListEntry = Cast<UGridBasedListEntry>(ActiveWidgets[DetectListItem]);
		if (!ensure(DetectListEntry))
		{
			return false;
		}

		const FIntPoint DraggedItemTopLeft = InDraggedItem->TopLeftPos;

		TArray<int32> Indexes;
		GetIndexes(OUT Indexes, InTargetTopLeft, InDraggedItem->TileSize);
		
		/* 
		*	현재 드래그 하고 있는 아이템의 TopLeft에 Detected된 아이템이 들어 올 것이기 때문에,
		*	Indexes (드래그 되고 있는 아이템이 옮겨질 위치에서로 부터 내가 차지할 공간)에 
		*	드래그 전 내가 차지하고 있는 공간이 겹치는지 확인이 필요하다.
		*/

		if (false == Indexes.Contains(MakeKey(DraggedItemTopLeft.X, DraggedItemTopLeft.Y)) &&
			true == IsEmptySpace(DraggedItemTopLeft, DetectListItem->TileSize) &&
			true == IsEmptySpace(InTargetTopLeft, InDraggedItem->TileSize, DetectListItem))
		{
			*OutSwappableEntry = DetectListEntry;
			return true;
		}
	}


	return false;
}

void UGridBasedListView::Swap(UGridBasedListEntry* InFirst, UGridBasedListEntry* InSecond, const FIntPoint& InDraggedTopLeft)
{
	UGridBasedListItem* FirstItem = nullptr != InFirst && ActiveItems.Contains(InFirst) ? ActiveItems[InFirst] : nullptr;
	UGridBasedListItem* SecondItem = nullptr != InSecond && ActiveItems.Contains(InSecond) ? ActiveItems[InSecond] : nullptr;

	if (!ensure(FirstItem && SecondItem))
	{
		return;
	}

	ForEach(FirstItem->TopLeftPos.X, FirstItem->TopLeftPos.Y, FirstItem->TileSize, [&](int32 InIndex)
		{
			if (Grid.IsValidIndex(InIndex))
			{
				Grid[InIndex] = nullptr;
			}
		});

	ForEach(SecondItem->TopLeftPos.X, SecondItem->TopLeftPos.Y, SecondItem->TileSize, [&](int32 InIndex)
		{
			if (Grid.IsValidIndex(InIndex))
			{
				Grid[InIndex] = nullptr;
			}
		});

	const FIntPoint CachedFirstTopLeft = FirstItem->TopLeftPos;

	// 위치 세팅
	SetItemPosition(FirstItem, InDraggedTopLeft);
	SetItemPosition(SecondItem, CachedFirstTopLeft);

	OnSwappedItem.Broadcast(FirstItem, SecondItem);
}

void UGridBasedListView::SetItemPosition(UGridBasedListItem* InItem, const FIntPoint& InPosition)
{
	if (nullptr != InItem)
	{
		ForEach(InPosition.X, InPosition.Y, InItem->TileSize, [&](int32 InIndex)
			{
				if (Grid.IsValidIndex(InIndex))
				{
					Grid[InIndex] = InItem;
				}
			});


		InItem->TopLeftPos = InPosition;

		if (!ensureAlways(ActiveWidgets.Contains(InItem)))
		{
			return;
		}

		UGridBasedListEntry* ListEntry = Cast<UGridBasedListEntry>(ActiveWidgets[InItem]);

		if (!ensureAlways(ListEntry))
		{
			return;
		}

		UCanvasPanelSlot* PanelSlot = InventoryPanel->AddChildToCanvas(ListEntry);
		if (ensure(PanelSlot))
		{
			PanelSlot->SetPosition(PointToLocal(InPosition));
			FVector2D Size = FVector2D(InItem->TileSize.X * SlotSize, InItem->TileSize.Y * SlotSize);
			PanelSlot->SetSize(Size);
		}
	}

}

void UGridBasedListView::Drop(UGridBasedListItem* InItem)
{
	if (nullptr == InItem)
	{
		return;
	}

	// 위치 복구
	SetItemPosition(InItem, InItem->TopLeftPos);

	/*ForEach(ListItem->TopLeftPos.X, ListItem->TopLeftPos.Y, ListItem->TileSize, [&](int32 InIndex)
		{
			if (Grid.IsValidIndex(InIndex))
			{
				Grid[InIndex] = ListItem;
			}
		});

	UCanvasPanelSlot* PanelSlot = InventoryPanel->AddChildToCanvas(InEntry);
	if (nullptr != PanelSlot)
	{
		PanelSlot->SetPosition(FVector2D(ListItem->TopLeftPos.X * GetSlotSize(), ListItem->TopLeftPos.Y * GetSlotSize()));
		FVector2D Size = FVector2D(ListItem->TileSize.X * GetSlotSize(), ListItem->TileSize.Y * GetSlotSize());
		PanelSlot->SetSize(Size);
	}*/

	OnDropItem.Broadcast(InItem);
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
			OnSelectionChanged.Broadcast(ListItem, false);
		}

		/*else if (true == DeActivedSelectedItems.Contains(InItem))
		{
			DeActivedSelectedItems.Remove(InItem);
		}*/

		else
		{
			SelectedItems.Emplace(InItem);
			OnSelectionChanged.Broadcast(ListItem, true);
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