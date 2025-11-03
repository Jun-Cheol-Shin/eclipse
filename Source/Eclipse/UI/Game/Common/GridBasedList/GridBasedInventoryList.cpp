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

	int32 PossibleTopLeftKey = GetBlankedSpaceIndex(InItem->Size.X, InItem->Size.Y);
	if (INDEX_NONE == PossibleTopLeftKey)
	{
		UE_LOG(LogTemp, Error, TEXT("Not enough empty space.."));
		return;
	}

	int32 Row = PossibleTopLeftKey % RowCount;
	int32 Column = PossibleTopLeftKey / RowCount;

	for (int i = 0; i < InItem->Size.Y; ++i)
	{
		for (int j = 0; j < InItem->Size.X; ++j)
		{
			int32 NewIndex = (Row + j) + ((Column + i) * RowCount);

			if (ensureAlways(Grid.IsValidIndex(NewIndex)))
			{
				Grid[NewIndex] = InItem;
			}
		}
	}


	if (!ensure(ScrollBox)) { return; }
	float ScrollOffset = ScrollBox->GetScrollOffset();
	int32 Height = PossibleTopLeftKey / RowCount;

	if (Height <= ScrollOffset)
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
	int32 BottomScrollOffset = FMath::RoundToInt32(InScrollOffset);
	int32 TopScrollOffset = BottomScrollOffset - ColumnCount;

	// remove item widget
	for (int i = 0; i < RowCount; ++i)
	{
		int NewKey = MakeKey(i, TopScrollOffset);
		if (!Grid.IsValidIndex(NewKey)) { continue; }

		UGridBasedInventoryItem* Item = Grid[NewKey];
		if (nullptr != Item && Item->Size.Y <= 1 && ActiveWidgets.Contains(Item))
		{
			TWeakObjectPtr<UUserWidget> ActivedWidget = ActiveWidgets[Item];
			if (ActivedWidget.IsValid())
			{
				IGridBasedObjectListEntry* InterfaceEntry = Cast<IGridBasedObjectListEntry>(ActivedWidget);
				if (ensure(InterfaceEntry))
				{
					InterfaceEntry->NativeOnEntryReleased();
				}

				ItemPool.Release(ActivedWidget.Get());
				ActiveWidgets.Remove(Item);
			}
		}
	}

	// add item widget
	for (int i = 0; i < RowCount; ++i)
	{
		int NewKey = MakeKey(i, BottomScrollOffset);
		if (!Grid.IsValidIndex(NewKey)) { continue; }

		UGridBasedInventoryItem* Item = Grid[NewKey];
		if (nullptr != Item && false == ActiveWidgets.Contains(Item))
		{
			UUserWidget* ActivedWidget = ItemPool.GetOrCreateInstance(ItemWidgetClass);
			if (ensure(nullptr != ActivedWidget))
			{
				IGridBasedObjectListEntry* InterfaceEntry = Cast<IGridBasedObjectListEntry>(ActivedWidget);
				if (ensure(InterfaceEntry))
				{
					InterfaceEntry->NativeOnListItemObjectSet(Item);
				}
			}

			if (InventoryPanel)
			{
				UCanvasPanelSlot* PanelSlot = InventoryPanel->AddChildToCanvas(ActivedWidget);
				if (ensure(PanelSlot))
				{
					PanelSlot->SetPosition(GetPosition(Item->Row, Item->Column));
				}
			}
		}
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
	}

	SetInventorySize();
	SetMaterial();

#if WITH_EDITOR
	MyCmdHandle = IConsoleManager::Get().RegisterConsoleCommand(
		TEXT("Inventory.AddItem"),
		TEXT("Add Item <Row Size> <Column Size>"),
		FConsoleCommandWithArgsDelegate::CreateWeakLambda(this, [this](const TArray<FString>& InArgs)
			{
				if (InArgs.Num() < 2)
				{
					UE_LOG(LogTemp, Warning, TEXT("usage: MyGame.SetValue <Name> <Value>"));
					return;
				}

				int Row = FCString::Atoi(*InArgs[0]);
				int Column = FCString::Atoi(*InArgs[1]);

				if (UGridBasedInventoryItem* NewItem = NewObject<UGridBasedInventoryItem>())
				{
					NewItem->Size = FVector2D(Row, Column);
					AddItem(NewItem);
				}

			}),
		ECVF_Default);
#endif
}

void UGridBasedInventoryList::NativeDestruct()
{
	ItemPool.ReleaseAll(true);
	ItemPool.ResetPool();

	Grid.Reset();

#if WITH_EDITOR
	IConsoleManager::Get().UnregisterConsoleObject(MyCmdHandle);
#endif


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
		IGridBasedObjectListEntry* InterfaceEntry = Cast<IGridBasedObjectListEntry>(ActivedWidget);
		if (ensure(InterfaceEntry))
		{
			InterfaceEntry->NativeOnListItemObjectSet(InItem);
		}
	}

	if (InventoryPanel)
	{
		UCanvasPanelSlot* PanelSlot = InventoryPanel->AddChildToCanvas(ActivedWidget);
		if (ensure(PanelSlot))
		{
			PanelSlot->SetPosition(GetPosition(InItem->Row, InItem->Column));

			FVector2D Size = FVector2D(InItem->Size.X * SlotSize, InItem->Size.Y * SlotSize);
			PanelSlot->SetSize(Size);
		}
	}
}

int32 UGridBasedInventoryList::MakeKey(uint32 InRow, uint32 InColumn)
{
	return InRow + (InColumn * RowCount);
}

int32 UGridBasedInventoryList::GetBlankedSpaceIndex(uint32 InSlotW, uint32 InSlotH)
{
	TQueue<int32> Queue;

	for (int i = 0; i < Grid.Num(); ++i)
	{
		if (nullptr == Grid[i])
		{
			Queue.Enqueue(i);
		}
	}

	while (false == Queue.IsEmpty())
	{
		int32 TopLeft;
		Queue.Dequeue(OUT TopLeft);

		TArray<int32> Indexes;

		int Row = TopLeft % RowCount;
		int Column = TopLeft / RowCount;

		bool IsEmpty = true;

		for (uint32 i = 0; i <= InSlotH - 1; ++i)
		{
			for (uint32 j = 0; j <= InSlotW - 1; ++j)
			{
				int NewIndex = (Row + j) + ((Column + i) * RowCount);

				if (Grid.IsValidIndex(NewIndex) && nullptr != Grid[NewIndex])
				{
					IsEmpty = false;
					break;
				}
			}

			if (false == IsEmpty)
			{
				break;
			}
		}

		if (true == IsEmpty)
		{
			return TopLeft;
		}
	}

	return INDEX_NONE;
}

FVector2D UGridBasedInventoryList::GetPosition(uint32 InSlotW, uint32 InSlotH)
{
	return FVector2D(SlotSize * InSlotW, SlotSize * InSlotH);
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
