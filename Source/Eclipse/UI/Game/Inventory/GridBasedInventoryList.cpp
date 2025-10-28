// Fill out your copyright notice in the Description page of Project Settings.


#include "GridBasedInventoryList.h"
#include "GridBasedInventoryEntry.h"

#include "CommonHierarchicalScrollBox.h"

#include "Components/SizeBox.h"
#include "Components/Border.h"

void UGridBasedInventoryList::OnChangedScrollOffset(float InScrollOffset)
{

}

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

	SetInventorySize();
	SetMaterial();
}

void UGridBasedInventoryList::NativeDestruct()
{
	SlotPool.ReleaseAll(true);
	SlotPool.ResetPool();

	Super::NativeDestruct();
}

void UGridBasedInventoryList::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	SetInventorySize();
	SetMaterial();	
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
}
