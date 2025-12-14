// Fill out your copyright notice in the Description page of Project Settings.


#include "EpInventoryComponent.h"
#include "EclipseInventoryItem.h"

#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"

#include "../../../Actor/EpDropItemActor.h"
#include "Eclipse/EclipseFunctionLibrary.h"
#include "Eclipse/Subsystems/EpGameDataSubSystem.h"
#include "Eclipse/DataTable/ItemShapeDataRow.h"


// Start EclipseInventoryEntry
FString FEclipseInventoryEntry::GetDebugString()
{
	return FString::Printf(TEXT("%s"), *GetNameSafe(ItemInstance));
}
// End EclipseInventroyEntry


// Start EclipseInventoryArray

void FEclipseInventoryArray::SetSize(const FIntPoint& InSize)
{
	if (!ensureAlways(OwnerComponent))
	{
		return;
	}

	if (OwnerComponent->GetOwner() && OwnerComponent->GetOwner()->HasAuthority())
	{
		ItemGrid.SetNumZeroed(InventorySize.X * InventorySize.Y);
	}
}

void FEclipseInventoryArray::PreReplicatedRemove(const TArrayView<int32>& RemovedIndices, int32 FinalSize)
{
	if (!ensureAlways(OwnerComponent))
	{
		return;
	}

	/*
	*	그리드 인벤토리는 배열 자리를 미리 할당 시킨다.
	*	Remove가 들어오는 경우는 인벤토리 확장 혹은 초기 세팅말고는 없는 것 같다.
	*/
}
void FEclipseInventoryArray::PostReplicatedAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize)
{
	if (!ensureAlways(OwnerComponent))
	{
		return;
	}

	/*	그리드 인벤토리는 배열 자리를 미리 할당 시킨다.
	*	Add가 들어오는 경우는 인벤토리 확장 혹은 초기 세팅말고는 없는 것 같다.
	*/
}
void FEclipseInventoryArray::PostReplicatedChange(const TArrayView<int32>& ChangedIndices, int32 FinalSize)
{
	if (!ensureAlways(OwnerComponent))
	{
		return;
	}

	int32 TopLeft = INDEX_NONE;
	UEclipseInventoryItem* AddedItem = nullptr;




	for (int32 Index : ChangedIndices)
	{
		FEclipseInventoryEntry& Entry = ItemGrid[Index];

		if (nullptr != Entry.ItemInstance)
		{
			AddedItem = Entry.ItemInstance;
			break;
		}
	}


	if (UEpInventoryComponent* InventoryComponent = Cast<UEpInventoryComponent>(OwnerComponent))
	{
		if (nullptr != AddedItem)
		{
			InventoryComponent->OnAddedItemDelegate.Broadcast(AddedItem);
		}

		else
		{
			// TopLeft는 가장 작은 원소
			int32* Min = Algo::MinElement(ChangedIndices);
			InventoryComponent->OnRemovedItemDelegate.Broadcast(*Min);
		}
	}
}

UEclipseInventoryItem* FEclipseInventoryArray::AddItem(int32 InItemId, int32 InStackCount)
{
	/*if (OwnerComponent && ENetRole::ROLE_Authority != CachedOwnerComponent->GetOwnerRole())
	{

		*	이 함수는 Inventory Component의 Server_AddItem으로 부터 호출된다.
		*	그렇기 때문에, 서버에서만 호출되며, 호출 되어야만한다.

		return;
	}*/

	if (!ensureAlways(OwnerComponent))
	{
		return nullptr;
	}

	UEclipseInventoryItem* Retval = nullptr;
	UEpGameDataSubsystem* GameDataManager = UEclipseFunctionLibrary::GetGameDataSubSytem(OwnerComponent->GetWorld());
	if (!ensure(GameDataManager))
	{
		return nullptr;
	}

	const FItemShapeDataRow* ItemShapeData = GameDataManager->GetGameData<FItemShapeDataRow>(InItemId);
	if (!ensure(ItemShapeData))
	{
		return nullptr;
	}


	TArray<int32> IndexList;
	int TopLeft = GetEmptyIndex(OUT IndexList, ItemShapeData->GetShapeSize());

	if (INDEX_NONE != TopLeft)
	{
		Retval = NewObject<UEclipseInventoryItem>(OwnerComponent->GetOwner());

		if (Retval)
		{
			Retval->SetItem(InItemId, InStackCount);
			Retval->SetPosition(TopLeft);
			for (int Index : IndexList)
			{
				FEclipseInventoryEntry NewEntry = ItemGrid[Index];
				NewEntry.ItemInstance = Retval;
			}

			Items.Emplace(Retval);

			MarkArrayDirty();
		}
	}

	return Retval;
}

void FEclipseInventoryArray::RemoveItem(UEclipseInventoryItem* InItem)
{
	if (!ensureAlways(OwnerComponent))
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
			FEclipseInventoryEntry NewEntry = ItemGrid[Index];
			
			if (ensure(NewEntry.ItemInstance == InItem))
			{
				NewEntry.ItemInstance = nullptr;
			}
		}

		Items.Remove(InItem);

		MarkArrayDirty();
	}

}

TArray<UEclipseInventoryItem*> FEclipseInventoryArray::GetAllItems() const
{
	/*TSet<UEclipseInventoryItem*> Results;
	Results.Reserve(ItemGrid.Num());

	for (const FEclipseInventoryEntry& Entry : ItemGrid)
	{
		if (Entry.ItemInstance != nullptr) //@TODO: Would prefer to not deal with this here and hide it further?
		{
			Results.Emplace(Entry.ItemInstance);
		}
	}

	return Results.Array();*/

	return Items.Array();
}


bool FEclipseInventoryArray::IsEmpty(int Index) const
{
	return ItemGrid.IsValidIndex(Index) && nullptr != ItemGrid[Index].ItemInstance;
}
int32 FEclipseInventoryArray::GetEmptyIndex(OUT TArray<int32>& OutIndexList, const FIntPoint& InItemSize) const
{
	TQueue<int32> TopLeftQueue;

	for (int i = 0; i < ItemGrid.Num(); ++i)
	{
		if (nullptr == ItemGrid[i].ItemInstance)
		{
			TopLeftQueue.Enqueue(i);
		}
	}

	OutIndexList.Reserve(InItemSize.X * InItemSize.Y);

	while (false == TopLeftQueue.IsEmpty())
	{
		int32 TopLeft;
		TopLeftQueue.Dequeue(OUT TopLeft);

		FIntPoint Point = IndexToPoint(TopLeft);

		if (true == IsEmptySpace(Point, InItemSize))
		{
			if (ensure(true == GetIndex(OUT OutIndexList, Point, InItemSize)))
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

			if (ItemGrid.IsValidIndex(Index) && nullptr != ItemGrid[Index].ItemInstance)
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


// Sets default values for this component's properties
UEpInventoryComponent::UEpInventoryComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, InventoryItemArray(this)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	//PrimaryComponentTick.bCanEverTick = true;
	//bWantsInitializeComponent = true;

	bReplicateUsingRegisteredSubObjectList = true;
	SetIsReplicatedByDefault(true);
}

void UEpInventoryComponent::InitializeComponent()
{
	Super::InitializeComponent();

	if (GetOwner()->HasAuthority())
	{
		InventoryItemArray.SetSize(InventorySize);
	}
}

void UEpInventoryComponent::Server_AddItem_Implementation(int32 InItemId, int32 InStackCount)
{
	if (GetOwner()->HasAuthority())
	{
		UEclipseInventoryItem* NewItem = nullptr;
		NewItem = InventoryItemArray.AddItem(InItemId, InStackCount);

		if (IsUsingRegisteredSubObjectList() && IsReadyForReplication() && NewItem)
		{
			AddReplicatedSubObject(NewItem);
		}
	}
}

bool UEpInventoryComponent::Server_AddItem_Validate(int32 InItemId, int32 InStackCount)
{
	if (0 >= InItemId || 0 >= InStackCount)
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

		if (InRemovedItem && IsUsingRegisteredSubObjectList())
		{
			RemoveReplicatedSubObject(InRemovedItem);
		}
	}
}

void UEpInventoryComponent::ReadyForReplication()
{
	Super::ReadyForReplication();

	if (IsUsingRegisteredSubObjectList())
	{
		for (UEclipseInventoryItem* ItemInstance : InventoryItemArray.GetAllItems())
		{
			if (IsValid(ItemInstance))
			{
				AddReplicatedSubObject(ItemInstance);
			}
		}
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

