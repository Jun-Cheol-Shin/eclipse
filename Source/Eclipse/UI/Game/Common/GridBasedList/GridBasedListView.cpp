// Fill out your copyright notice in the Description page of Project Settings.


#include "GridBasedListView.h"


#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "CommonHierarchicalScrollBox.h"

#include "Components/SizeBox.h"
#include "Components/Border.h"

#include "GridBasedObjectListEntry.h"

#include "HAL/IConsoleManager.h"

void UGridBasedListView::NativeOnStartDetectDrag(UUserWidget* InDraggingWidget, const FPointerEvent& InEvent)
{
	IDragDetectable::NativeOnStartDetectDrag(InDraggingWidget, InEvent);

	UE_LOG(LogTemp, Warning, TEXT("On Start Detect! GridBasedListView"));

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

	FootprintWidgetSlot->SetSize(GetLocalFromIndex(ListItem->TileSize.X, ListItem->TileSize.Y));

	const FVector2D CursorScreenSpacePos = InEvent.GetScreenSpacePosition();
	const FVector2D InventorySpacePos = InventoryPanel->GetTickSpaceGeometry().AbsoluteToLocal(CursorScreenSpacePos);

	int32 XIdx = 0;
	int32 YIdx = 0;

	ForEach(ListItem->TopLeftPos.X, ListItem->TopLeftPos.Y, ListItem->TileSize, [&](int32 InIndex)
		{
			if (Grid.IsValidIndex(InIndex))
			{
				Grid[InIndex] = nullptr;
			}
		});

	if (true == GetIndexFromLocal(InventorySpacePos, OUT XIdx, OUT YIdx, ListItem->TileSize))
	{
		//UE_LOG(LogTemp, Warning, TEXT("Screen = %.1f, %.1f, Inventory = %.1f, %.1f"), CursorScreenSpacePos.X, CursorScreenSpacePos.Y, InventorySpacePos.X, InventorySpacePos.Y);
		//UE_LOG(LogTemp, Warning, TEXT("Index = %d, %d"), WidthIdx, HeightIdx);

		bool bIsEmpty = true == IsEmptySpace(XIdx, YIdx, ListItem->TileSize);

		if (true == bIsEmpty)
		{
			FootprintWidget->SetStyle(FootprintStyles[EStorageState::Empty]);
		}

		else
		{
			FootprintWidget->SetStyle(FootprintStyles[EStorageState::IsFull]);
		}

		FootprintWidgetSlot->SetPosition(GetLocalFromIndex(XIdx, YIdx));
		TempFootprintLoc = FIntPoint(XIdx, YIdx);
	}

}

void UGridBasedListView::NativeOnDetect(UUserWidget* InDraggingWidget, const FPointerEvent& InEvent)
{
	IDragDetectable::NativeOnDetect(InDraggingWidget, InEvent);

	UE_LOG(LogTemp, Warning, TEXT("On Detect! GridBasedListView"));

	if (nullptr == InventoryPanel || nullptr == FootprintWidget) { return; }

	UCanvasPanelSlot* FootprintWidgetSlot = Cast<UCanvasPanelSlot>(FootprintWidget->Slot);
	if (nullptr == FootprintWidgetSlot) return;

	UGridBasedListEntry* ListEntry = Cast<UGridBasedListEntry>(InDraggingWidget);
	if (nullptr == ListEntry) { return; }

	const UGridBasedListItem* ListItem = GetItemFromListEntry(ListEntry);
	if (nullptr == ListItem) { return; }

	const FVector2D CursorScreenSpacePos = InEvent.GetScreenSpacePosition();
	const FVector2D InventorySpacePos = InventoryPanel->GetTickSpaceGeometry().AbsoluteToLocal(CursorScreenSpacePos);

	int32 XIdx = 0;
	int32 YIdx = 0;

	if (true == GetIndexFromLocal(InventorySpacePos, OUT XIdx, OUT YIdx, ListItem->TileSize))
	{
		bool bIsEmpty = true == IsEmptySpace(XIdx, YIdx, ListItem->TileSize);

		if (true == bIsEmpty)
		{
			FootprintWidget->SetStyle(FootprintStyles[EStorageState::Empty]);
		}

		else
		{
			FootprintWidget->SetStyle(FootprintStyles[EStorageState::IsFull]);
		}

		FootprintWidgetSlot->SetPosition(GetLocalFromIndex(XIdx, YIdx));
		TempFootprintLoc = FIntPoint(XIdx, YIdx);
	}
}

void UGridBasedListView::NativeOnEndDetect(UUserWidget* InDraggingWidget, const FPointerEvent& InEvent)
{
	IDragDetectable::NativeOnEndDetect(InDraggingWidget, InEvent);

	UE_LOG(LogTemp, Warning, TEXT("On End Detect! GridBasedListView"));

	if (nullptr != FootprintWidget)
	{
		FootprintWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UGridBasedListView::NativeOnDrop(UUserWidget* InDraggingWidget, const FPointerEvent& InEvent)
{
	IDragDetectable::NativeOnDrop(InDraggingWidget, InEvent);

	UE_LOG(LogTemp, Warning, TEXT("On Drop! GridBasedListView"));

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

	bool bIsEmpty = IsEmptySpace(TempFootprintLoc.X, TempFootprintLoc.Y, ListItem->TileSize);

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
		PanelSlot->SetPosition(GetLocalFromIndex(DropPos.X, DropPos.Y));
		FVector2D Size = FVector2D(ListItem->TileSize.X * SlotSize, ListItem->TileSize.Y * SlotSize);
		PanelSlot->SetSize(Size);
	}

	TempFootprintLoc = FIntPoint(-1, -1);
}

void UGridBasedListView::AddItem(UGridBasedListItem* InItem)
{
	if (!ensure(InItem))
	{
		return;
	}
	
	TArray<int32> GridList;
	int32 PossibleTopLeftKey = GetBlankedSpaceIndex(OUT GridList, InItem->TileSize.X, InItem->TileSize.Y);
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

		RemoveWidget(InItem);
		TWeakObjectPtr<UUserWidget> FoundWidget = ActiveWidgets.FindRef(InItem);

		if (FoundWidget.IsValid())
		{
			ActiveWidgets.Remove(InItem);
			ActiveItems.Remove(FoundWidget.Get());
		}

		ListItems.Remove(InItem);
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
	Super::NativeDestruct();

	ItemPool.ReleaseAll(true);
	ItemPool.ResetPool();

	Grid.Reset();
	ListItems.Reset();
	ActiveWidgets.Reset();
	ActiveItems.Reset();
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
				PanelSlot->SetPosition(GetLocalFromIndex(InItem->TopLeftPos.X, InItem->TopLeftPos.Y));

				FVector2D Size = FVector2D(InItem->TileSize.X * SlotSize, InItem->TileSize.Y * SlotSize);
				PanelSlot->SetSize(Size);
			}
		}

		ActiveWidgets.Emplace(InItem, ActivedWidget);
		ActiveItems.Emplace(ActivedWidget, InItem);
	}
}

void UGridBasedListView::RemoveWidget(const UGridBasedListItem* InItem)
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

FVector2D UGridBasedListView::GetLocalFromIndex(int32 InX, int32 InY)
{
	return FVector2D(SlotSize * InX, SlotSize * InY);
}

bool UGridBasedListView::GetIndexFromLocal(const FVector2D InPos, OUT int32& OutX, int32& OutY, const FIntPoint& InLocalSize)
{
	int Width = InPos.X / SlotSize;
	int Height = InPos.Y / SlotSize;

	if (Width < 0 || Height < 0 || Width >= RowCount || Height >= ColumnCount)
	{
		return false;
	}

	/*if (Width >= 0 && Height >= 0 && Width < RowCount && Height < ColumnCount)
	{

		return true;
	}*/

	if (Width + InLocalSize.X > RowCount)
	{
		Width = RowCount - InLocalSize.X;
	}

	if (Height + InLocalSize.Y > ColumnCount)
	{
		Height = ColumnCount - InLocalSize.Y;
	}

	OutX = Width;
	OutY = Height;

	return true;
}

int32 UGridBasedListView::GetBlankedSpaceIndex(OUT TArray<int32>& OutGridList, int8 InX, int8 InY)
{
	TQueue<int32> Queue;

	for (int i = 0; i < Grid.Num(); ++i)
	{
		if (nullptr == Grid[i])
		{
			Queue.Enqueue(i);
		}
	}

	OutGridList.Reserve(InX * InY);

	while (false == Queue.IsEmpty())
	{
		int32 TopLeft;
		Queue.Dequeue(OUT TopLeft);

		int Level = TopLeft / RowCount;
		if (Level != (TopLeft + InY - 1) / RowCount)
		{
			continue;
		}

		OutGridList.Reset();

		bool flag = false;

		for (int i = 0; i < InY; ++i)
		{
			int Height = i * RowCount;

			for (int j = 0; j < InX; ++j)
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

		if (OutGridList.Num() == InX * InY)
		{
			return OutGridList.IsValidIndex(0) ? OutGridList[0] : INDEX_NONE;
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

bool UGridBasedListView::IsEmptySpace(int32 InX, int32 InY, const FIntPoint& InSize)
{
	for (int32 i = 0; i < InSize.Y; ++i)
	{
		for (int32 j = 0; j < InSize.X; ++j)
		{
			int32 Index = MakeKey(InX + j, InY + i);

			if (Grid.IsValidIndex(Index) &&
				nullptr != Grid[Index])
			{
				return false;
			}
		}
	}


	return true;
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

float UGridBasedListView::GetSlotSize() const
{
	return SlotSize;
}