// Fill out your copyright notice in the Description page of Project Settings.


#include "GridBasedListView.h"


#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "CommonHierarchicalScrollBox.h"

#include "Components/SizeBox.h"
#include "Components/Border.h"

#include "GridBasedObjectListEntry.h"

#include "HAL/IConsoleManager.h"

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

	int32 TopLeftX = PossibleTopLeftKey % RowCount;
	int32 TopLeftY = PossibleTopLeftKey / RowCount;

	for (int32 Index : GridList)
	{
		Grid[Index] = InItem;
	}

	InItem->TopLeftPos = FDoubleArrayIndexes(TopLeftX, TopLeftY);

	if (false == IsOverScroll(PossibleTopLeftKey))
	{
		AddWidget(InItem);
	}
}

void UGridBasedListView::RemoveItem(UGridBasedListItem* InItem)
{
	// TODO

}

UUserWidget* UGridBasedListView::GetEntry(UGridBasedListItem* InItem)
{
	// TODO

	return nullptr;
}

UGridBasedListItem* UGridBasedListView::GetItemFromListEntry(UUserWidget* InWidget)
{
	// TODO
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

			UGridBasedListItem* Item = Grid[NewKey];

			// TODO : 아이템의 TopLeft.Y + Size.Y가 아이템이 차지하는 공간의 밑부분
			// TopLeft : 0 , Size.Y = 5라면, 5열이 아이템의 밑부분이다.
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

			UGridBasedListItem* Item = Grid[NewKey];
			if (nullptr != Item && false == ActiveWidgets.Contains(Item))
			{
				AddWidget(Item);
			}
		}

		CurrentPage = NewCurrentPageIndex;
	}
	
	if (GEngine && InventoryPanel)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("Current Height = %d, Active Widget Num = %d"), NewCurrentPageIndex, InventoryPanel->GetChildrenCount()));
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
}

void UGridBasedListView::NativeDestruct()
{
	ItemPool.ReleaseAll(true);
	ItemPool.ResetPool();

	Grid.Reset();


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
			InterfaceEntry->NativeOnListItemObjectSet(InItem);
			InterfaceEntry->OwningListView = this;
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
				PanelSlot->SetPosition(IndexToLocalSize(InItem->TopLeftPos.X, InItem->TopLeftPos.Y));

				FVector2D Size = FVector2D(InItem->TileSize.X * SlotSize, InItem->TileSize.Y * SlotSize);
				PanelSlot->SetSize(Size);
			}
		}

		ActiveWidgets.Emplace(InItem, ActivedWidget);
	}
}

void UGridBasedListView::RemoveWidget(UGridBasedListItem* InItem)
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
		ActiveWidgets.Remove(InItem);
	}
}

int32 UGridBasedListView::MakeKey(uint32 InRow, uint32 InColumn)
{
	return InRow + (InColumn * RowCount);
}

FVector2D UGridBasedListView::IndexToLocalSize(uint32 InSlotW, uint32 InSlotH)
{
	return FVector2D(SlotSize * InSlotW, SlotSize * InSlotH);
}

int32 UGridBasedListView::GetBlankedSpaceIndex(OUT TArray<int32>& OutGridList, uint8 InWidth, uint8 InHeight)
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
			for (FDoubleArrayIndexes& Indexes : HiddenIndex)
			{
				int X = FMath::Clamp(Indexes.X, 0, RowCount);
				int Y = FMath::Clamp(Indexes.Y, 0, ColumnCount);

				Index.Emplace(X + RowCount * Y);
			}

			// 0~127 = 32비트*4 = RGBA
			uint32 W0 = 0, W1 = 0, W2 = 0, W3 = 0;

			for (int32 i : Index)
			{
				if (i < 0 || i >= 128) continue;       // A만 쓰므로 범위를 0..127로 제한
				const uint32 word = (uint32)i >> 5;    // 0..3 (각 32칸)
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

			// uint32를 float로 비트 재해석해 Vector4로 패킹
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

float UGridBasedListView::GetSlotSize() const
{
	return SlotSize;
}