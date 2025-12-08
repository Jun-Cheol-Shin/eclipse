// Fill out your copyright notice in the Description page of Project Settings.


#include "EpInventoryComponent.h"
#include "EclipseInventoryItem.h"

#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"

#include "../../../Actor/EpDropItemActor.h"

/*
static TAutoConsoleVariable<int32> CVarShowInventory(
	TEXT("ShowDebugInventory"),
	0,
	TEXT("Draws debug info about inventory")
	TEXT(" 0 : off\n")
	TEXT(" 1 : on\n"),
	ECVF_Cheat
);
*/

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

void UEpInventoryComponent::Server_RemoveItem_Implementation(UEclipseInventoryItem* InRemovedItem)
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

	switch (GetNetMode())
	{
	case ENetMode::NM_Client:
	{
		InventoryItemArray.SetComponent(this);
	}
		break;
	case ENetMode::NM_DedicatedServer:
	{
		InventoryItemArray.SetComponent(this);
		InventoryItemArray.SetInventory(InventorySize);
	}
		break;

	case ENetMode::NM_ListenServer:
	{
		InventoryItemArray.SetComponent(this);
		InventoryItemArray.SetInventory(InventorySize);
	}
		break;

	default:
	{
		// standalone?
		InventoryItemArray.SetComponent(this);
		InventoryItemArray.SetInventory(InventorySize);
	}
		break;
	}

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

/*
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
		}
	}
#endif
*/

}

// FEclipseInventoryArray
void FEclipseInventoryArray::SetInventory(const FIntPoint& InSize)
{
	InventorySize = InSize;
	Items.Reset();

	for (int i = 0; i < InSize.X * InSize.Y; ++i)
	{
		Items.Emplace(FEclipseInventoryEntry());
	}

	MarkArrayDirty();
}

void FEclipseInventoryArray::SetComponent(UActorComponent* InComponent)
{
	CachedOwnerComponent = InComponent;
}

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

	/*	
	*	그리드 인벤토리는 배열 자리를 미리 할당 시킨다.
	*	Remove가 들어오는 경우는 인벤토리 확장 혹은 초기 세팅말고는 없는 것 같다.
	*/

	/*UEpInventoryComponent* InventoryComp = Cast<UEpInventoryComponent>(CachedOwnerComponent);
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
	*/



}
void FEclipseInventoryArray::PostReplicatedAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize)
{
	if (!ensureAlways(CachedOwnerComponent.IsValid()))
	{
		return;
	}

	/*	그리드 인벤토리는 배열 자리를 미리 할당 시킨다.
	*	Add가 들어오는 경우는 인벤토리 확장 혹은 초기 세팅말고는 없는 것 같다.
	*/


	/*UEpInventoryComponent* InventoryComp = Cast<UEpInventoryComponent>(CachedOwnerComponent);
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
	}*/
}
void FEclipseInventoryArray::PostReplicatedChange(const TArrayView<int32>& ChangedIndices, int32 FinalSize)
{
	if (!ensureAlways(CachedOwnerComponent.IsValid()))
	{
		return;
	}

	// TODO : ChangedIndices에서 가장 작은 값을 가져온다 -> TopLeft을 가져오기




}

void FEclipseInventoryArray::AddItem(UEclipseInventoryItem* InItem)
{
	if (CachedOwnerComponent.IsValid() && ENetRole::ROLE_Authority != CachedOwnerComponent->GetOwnerRole())
	{
		/*
		*	이 함수는 Inventory Component의 Server_AddItem으로 부터 호출된다.
		*	그렇기 때문에, 서버에서만 호출되며, 호출 되어야만한다.
		*/
		return;
	}

	TArray<int32> IndexList;
	int TopLeft = GetEmptyIndex(OUT IndexList, InItem);

	if (INDEX_NONE != TopLeft)
	{
		InItem->SetPosition(TopLeft);

		for (int Index : IndexList)
		{
			FEclipseInventoryEntry NewEntry = Items[Index];
			NewEntry.ItemInstance = InItem;
		}

		MarkArrayDirty();
	}
}

void FEclipseInventoryArray::RemoveItem(UEclipseInventoryItem* InItem)
{
	if (!ensureAlways(CachedOwnerComponent.IsValid()))
	{
		return;
	}

	if (nullptr == InItem)
	{
		return;
	}

	int32 TopLeft = InItem->GetPosition();

	if (INDEX_NONE != TopLeft)
	{
		TArray<int32> IndexList;
		GetIndex(OUT IndexList, TopLeft, InItem->GetItemSize());

		for (int Index : IndexList)
		{
			FEclipseInventoryEntry NewEntry = Items[Index];
			NewEntry.ItemInstance = nullptr;
		}

		MarkArrayDirty();
	}
	
}

bool FEclipseInventoryArray::IsEmpty(int Index) const
{
	return Items.IsValidIndex(Index) && nullptr != Items[Index].ItemInstance;
}

bool FEclipseInventoryArray::IsValid() const
{
	return true == CachedOwnerComponent.IsValid() && InventorySize != FIntPoint(0, 0);
}

int32 FEclipseInventoryArray::GetEmptyIndex(OUT TArray<int32>& OutIndexList, UEclipseInventoryItem* InItem) const
{
	TQueue<int32> TopLeftQueue;

	for (int i = 0; i < Items.Num(); ++i)
	{
		if (nullptr == Items[i].ItemInstance)
		{
			TopLeftQueue.Enqueue(i);
		}
	}

	FIntPoint ItemSize = InItem->GetItemSize();

	OutIndexList.Reserve(ItemSize.X * ItemSize.Y);

	while (false == TopLeftQueue.IsEmpty())
	{
		int32 TopLeft;
		TopLeftQueue.Dequeue(OUT TopLeft);

		FIntPoint Point = IndexToPoint(TopLeft);

		if (true == IsEmptySpace(Point, ItemSize))
		{
			if (ensure(true == GetIndex(OUT OutIndexList, Point, ItemSize)))
			{
				return OutIndexList.IsValidIndex(0) ? OutIndexList[0] : INDEX_NONE;
			}
		}
	}

	return INDEX_NONE;
}

FIntPoint FEclipseInventoryArray::IndexToPoint(int32 Index) const
{
	return FIntPoint(Index % InventorySize.X, Index / InventorySize.X);
}

int32 FEclipseInventoryArray::PointToIndex(const FIntPoint& InPoint) const
{
	return InPoint.Y * InventorySize.X + InPoint.X;
}

bool FEclipseInventoryArray::IsEmptySpace(const FIntPoint& InTopLeftPoint, const FIntPoint& InSize) const
{
	for (int32 i = 0; i < InSize.Y; ++i)
	{
		if (InTopLeftPoint.Y + i >= InventorySize.Y)
		{
			return false;
		}

		for (int32 j = 0; j < InSize.X; ++j)
		{
			if (InTopLeftPoint.X + j >= InventorySize.X)
			{
				return false;
			}

			int32 Index = PointToIndex({ InTopLeftPoint.X + j, InTopLeftPoint.Y + i });

			if (Items.IsValidIndex(Index) && nullptr != Items[Index].ItemInstance)
			{
				return false;
			}
		}
	}

	return true;
}

bool FEclipseInventoryArray::GetIndex(OUT TArray<int32>& OutIndexList, const FIntPoint& InTopLeft, const FIntPoint& InSize) const
{
	OutIndexList.Reset();

	for (int i = 0; i < InSize.Y; ++i)
	{
		for (int j = 0; j < InSize.X; ++j)
		{
			OutIndexList.Emplace(PointToIndex({ InTopLeft.X + j, InTopLeft.Y + i }));
		}
	}

	return OutIndexList.Num() == InSize.X * InSize.Y;
}

// End FEclipseInventoryArray


FString FEclipseInventoryEntry::GetDebugString()
{
	return FString::Printf(TEXT("%s"), *GetNameSafe(ItemInstance));
}