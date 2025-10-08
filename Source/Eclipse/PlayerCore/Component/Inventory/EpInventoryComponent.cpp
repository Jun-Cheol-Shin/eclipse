// Fill out your copyright notice in the Description page of Project Settings.


#include "EpInventoryComponent.h"
#include "EclipseInventoryItem.h"

TArray<UEclipseInventoryItem*> FEclipseInventoryArray::GetAllItems() const
{
	TArray<UEclipseInventoryItem*> ItemList;
	ItemList.Reserve(Items.Num());

	Algo::Transform(Items, ItemList, [](const FEclipseInventoryEntry InEntry) -> UEclipseInventoryItem*
		{
			return InEntry.ItemInstance;
		});

	ItemList.RemoveAll([](UEclipseInventoryItem* InItem)
		{
			return nullptr == InItem;
		});

	return ItemList;
}

void FEclipseInventoryArray::PreReplicatedRemove(const TArrayView<int32>& RemovedIndices, int32 FinalSize)
{



}

void FEclipseInventoryArray::PostReplicatedAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize)
{



}

void FEclipseInventoryArray::PostReplicatedChange(const TArrayView<int32>& ChangedIndices, int32 FinalSize)
{
	

}

void FEclipseInventoryArray::AddItem(UEclipseInventoryItem* InItem)
{

}

void FEclipseInventoryArray::AddItem(UEpInventoryComponent* InComponent)
{

}

void FEclipseInventoryArray::RemoveItem(UEclipseInventoryItem* InItem)
{

}

// Sets default values for this component's properties
UEpInventoryComponent::UEpInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

