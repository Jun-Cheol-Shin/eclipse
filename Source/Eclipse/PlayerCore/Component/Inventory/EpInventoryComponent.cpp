// Fill out your copyright notice in the Description page of Project Settings.


#include "EpInventoryComponent.h"
#include "EclipseInventoryItem.h"

#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"

#include "../../../Actor/EpDropItemActor.h"


static TAutoConsoleVariable<int32> CVarShowInventory(
	TEXT("ShowDebugInventory"),
	0,
	TEXT("Draws debug info about inventory")
	TEXT(" 0 : off\n")
	TEXT(" 1 : on\n"),
	ECVF_Cheat
);

void UEpInventoryComponent::Server_AddItem_Implementation(UEclipseInventoryItem* InAddItem)
{
	if (GetOwner()->HasAuthority())
	{
		InventoryItemArray.AddItem(InAddItem);
	}
}

bool UEpInventoryComponent::Server_AddItem_Validate(UEclipseInventoryItem* InAddItem)
{
	if (nullptr == InAddItem)
	{
		return false;
	}

	if (0 >= InAddItem->GetItemId() || 0 >= InAddItem->GetItemStackCount())
	{
		return false;
	}

	return true;
}

void UEpInventoryComponent::RemoveItem(UEclipseInventoryItem* InRemovedItem)
{
	if (GetOwner()->HasAuthority())
	{
		InventoryItemArray.RemoveItem(InRemovedItem);
	}
}

const FIntPoint& UEpInventoryComponent::GetInventorySize() const
{
	return InventorySize;
}

bool UEpInventoryComponent::IsPossibleAdd() const
{
	return false;
}

// Sets default values for this component's properties
UEpInventoryComponent::UEpInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;
	SetIsReplicatedByDefault(true);

}

void UEpInventoryComponent::InitializeComponent()
{
	Super::InitializeComponent();

	UE_LOG(LogTemp, Warning, TEXT("Init Inventory Component!"));

	InventoryItemArray = FEclipseInventoryArray(this, { 12, 5 });
}

bool UEpInventoryComponent::ReplicateSubobjects(UActorChannel* InChannel, FOutBunch* InBunch, FReplicationFlags* InRepFlags)
{
	bool bWroteSomething = Super::ReplicateSubobjects(InChannel, InBunch, InRepFlags);

	for (UEclipseInventoryItem* Item : InventoryItemArray.GetAllItems())
	{
		if (nullptr != Item)
		{
			bWroteSomething |= InChannel->ReplicateSubobject(Item, *InBunch, *InRepFlags);
		}
	}

	return bWroteSomething;
}

void UEpInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams ConditionParam;
	ConditionParam.Condition = COND_OwnerOnly;
	ConditionParam.RepNotifyCondition = REPNOTIFY_OnChanged;

	DOREPLIFETIME_WITH_PARAMS_FAST(UEpInventoryComponent, InventoryItemArray, ConditionParam);
}

void UEpInventoryComponent::TickComponent(float InDeltaTime, ELevelTick InTickType, FActorComponentTickFunction* InTickFunc)
{
	Super::TickComponent(InDeltaTime, InTickType, InTickFunc);

#if WITH_EDITOR
	const bool bShowDebug = CVarShowInventory.GetValueOnGameThread() != 0;
	if (true == bShowDebug)
	{
		for (UEclipseInventoryItem* Item : InventoryItemArray.GetAllItems())
		{
			if (nullptr != Item)
			{
				GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Green, FString::Printf(TEXT("Item Name : %s, Item Count = %d"), *Item->GetItemNameStr(), Item->GetItemStackCount()));
			}

			else
			{
				//
			}
		}
	}
#endif

}

// FEclipseInventoryArray

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
	if (!ensureAlways(CachedOwnerComponent.IsValid()))
	{
		return;
	}


	UEpInventoryComponent* InventoryComp = Cast<UEpInventoryComponent>(CachedOwnerComponent);
	if (!ensureAlways(InventoryComp))
	{
		return;
	}

	for (int i = 0; i < RemovedIndices.Num(); ++i)
	{
		int Index = RemovedIndices[i];
		FEclipseInventoryEntry RemovedEntry = Items[Index];
		UEclipseInventoryItem* RemovedItem = RemovedEntry.ItemInstance;

		if (!ensure(RemovedItem))
		{
			UE_LOG(LogTemp, Error, TEXT("Invalid Removed Item %s"), ANSI_TO_TCHAR(__FUNCTION__));
			continue;
		}

		InventoryComp->OnRemovedItemDelegate.Broadcast(RemovedItem);
	}




}
void FEclipseInventoryArray::PostReplicatedAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize)
{
	if (!ensureAlways(CachedOwnerComponent.IsValid()))
	{
		return;
	}


	UEpInventoryComponent* InventoryComp = Cast<UEpInventoryComponent>(CachedOwnerComponent);
	if (!ensureAlways(InventoryComp))
	{
		return;
	}

	for (int i = 0; i < AddedIndices.Num(); ++i)
	{
		int Index = AddedIndices[i];
		FEclipseInventoryEntry AddedEntry = Items[Index];
		UEclipseInventoryItem* AddedItem = AddedEntry.ItemInstance;

		if (!ensureMsgf(AddedItem, TEXT("Invalid Added Item %s"), ANSI_TO_TCHAR(__FUNCTION__)))
		{
			continue;
		}

		InventoryComp->OnAddedItemDelegate.Broadcast(AddedItem);
	}
}
void FEclipseInventoryArray::PostReplicatedChange(const TArrayView<int32>& ChangedIndices, int32 FinalSize)
{

}

void FEclipseInventoryArray::AddItem(UEpInventoryComponent* InComponent)
{
	//unimplemented();
}
void FEclipseInventoryArray::AddItem(UEclipseInventoryItem* InItem)
{
	if (!ensureAlways(CachedOwnerComponent.IsValid()))
	{
		return;
	}


	AActor* OwningActor = CachedOwnerComponent->GetOwner();
	if (nullptr == OwningActor || false == OwningActor->HasAuthority())
	{
		return;
	}

	FEclipseInventoryEntry NewEntry = Items.AddDefaulted_GetRef();
	NewEntry.ItemInstance = InItem;

	MarkItemDirty(NewEntry);
}

void FEclipseInventoryArray::RemoveItem(UEclipseInventoryItem* InItem)
{
	for (auto EntryIt = Items.CreateIterator(); EntryIt; ++EntryIt)
	{
		FEclipseInventoryEntry& Entry = *EntryIt;
		UEclipseInventoryItem* Item = Entry.ItemInstance;

		if (nullptr == Item)
		{
			EntryIt.RemoveCurrent();
			MarkArrayDirty();
		}

		else if (Item->IsEqual(InItem))
		{
			EntryIt.RemoveCurrent();
			MarkArrayDirty();
		}
	}

}

bool FEclipseInventoryArray::IsEmpty(int Index) const
{
	return Items.IsValidIndex(Index) && nullptr != Items[Index].ItemInstance;
}

bool UEpInventoryComponent::IsPossibleAdd(UEclipseInventoryItem* InItem) const
{
	const FIntPoint TopLeft = InItem->GetTopLeft();
	const FIntPoint Size = InItem->GetItemSize();

	for (int32 i = 0; i < InventorySize.Y; ++i)
	{
		if (TopLeft.Y + i >= InventorySize.Y)
		{
			return false;
		}

		for (int32 j = 0; j < InventorySize.X; ++j)
		{
			if (TopLeft.X + j >= InventorySize.X)
			{
				return false;
			}

			int32 Index = GetIndex({ TopLeft.X + j, TopLeft.Y + i });

			if (false == InventoryItemArray.IsEmpty(Index))
			{
				return false;
			}
		}
	}


	return true;
}

int UEpInventoryComponent::GetIndex(const FIntPoint& InPoint) const
{
	return InPoint.X + (InPoint.Y * InventorySize.X);
}

// End FEclipseInventoryArray


FString FEclipseInventoryEntry::GetDebugString()
{
	return FString::Printf(TEXT("%s"), *GetNameSafe(ItemInstance));
}