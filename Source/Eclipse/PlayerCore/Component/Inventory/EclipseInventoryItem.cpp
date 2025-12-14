// Fill out your copyright notice in the Description page of Project Settings.


#include "EclipseInventoryItem.h"
#include "../../../GameModes/EpGameInstance.h"
#include "../../../Subsystems/EpGameDataSubSystem.h"

#include "../../../DataTable/ItemDataRow.h"
#include "../../../DataTable/ItemResourceDataRow.h"
#include "../../../DataTable/ItemShapeDataRow.h"

#include "Net/UnrealNetwork.h"

void UEclipseInventoryItem::SetItem(int32 InItemId, int64 InStackCount)
{
	//Serial = GetUniqueID();
	ItemId = InItemId;
	StackCount = StackCount;
}

void UEclipseInventoryItem::SetPosition(int32 InTopLeft)
{
	TopLeft = InTopLeft;
}

void UEclipseInventoryItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// TODO : Change Conditional

	DOREPLIFETIME(UEclipseInventoryItem, StackCount);
	//DOREPLIFETIME(UEclipseInventoryItem, Serial);
	DOREPLIFETIME(UEclipseInventoryItem, ItemId);
}


bool UEclipseInventoryItem::IsEqual(UEclipseInventoryItem* OtherItem)
{
	return	TopLeft == OtherItem->TopLeft &&
			ItemId == OtherItem->ItemId &&
			StackCount == OtherItem->StackCount;
}

FString UEclipseInventoryItem::GetItemNameStr() const
{
	const FItemDataRow* ItemData = GetItemData();
	if (ItemData)
	{
		return ItemData->ItemName;
	}

	return TEXT("Invalid Item Name");
}

EItemRarity UEclipseInventoryItem::GetRarity() const
{
	const FItemDataRow* ItemData = GetItemData();
	if (ItemData)
	{
		return ItemData->Rarity;
	}

	return EItemRarity::Common;
}

EClassType UEclipseInventoryItem::GetClassTypeEnum() const
{
	const FItemDataRow* ItemData = GetItemData();
	if (ItemData)
	{
		return static_cast<EClassType>(ItemData->Class);
	}

	return EClassType::None;
}

uint8 UEclipseInventoryItem::GetClassTypeBit() const
{
	const FItemDataRow* ItemData = GetItemData();
	if (ItemData)
	{
		return ItemData->Class;
	}

	return 0;
}

int64 UEclipseInventoryItem::GetMaxStackCount() const
{
	const FItemDataRow* ItemData = GetItemData();
	if (ItemData)
	{
		return ItemData->MaxStackCount;
	}

	return 0;
}

FSoftObjectPath UEclipseInventoryItem::GetMeshPath() const
{
	const FItemResourceDataRow* ResourceData = GetItemResourceData();
	if (ResourceData)
	{
		return ResourceData->StaticMeshPath;
	}
	
	return nullptr;
}

FSoftObjectPath UEclipseInventoryItem::GetThumbnailPath() const
{
	const FItemResourceDataRow* ResourceData = GetItemResourceData();
	if (ResourceData)
	{
		return ResourceData->ThumbnailImagePath;
	}

	return nullptr;
}

FIntPoint UEclipseInventoryItem::GetItemSize() const
{
	const FItemShapeDataRow* ShapeData = GetItemShapeData();
	if (ShapeData)
	{
		return FIntPoint(ShapeData->Width, ShapeData->Height);
	}

	return FIntPoint(0, 0);
}

TArray<int32> UEclipseInventoryItem::GetItemIndexList() const
{
	const FItemShapeDataRow* ShapeData = GetItemShapeData();
	if (ShapeData)
	{
		TArray<int32> IndexList;

		int Num = ShapeData->Height * ShapeData->Width;
		for (int i = 0; i < Num; ++i)
		{
			if (false == ShapeData->HiddenIndexList.Contains(i))
			{
				IndexList.Emplace(i);
			}
		}

		return IndexList;
	}

	return TArray<int32>();
}


const FItemDataRow* UEclipseInventoryItem::GetItemData() const
{
	UEpGameDataSubsystem* GameDataSubsystem = GetGameDataSubsystem();
	if (ensure(GameDataSubsystem))
	{
		return GameDataSubsystem->GetGameData<FItemDataRow>(ItemId);
	}

	return nullptr;
}

const FItemResourceDataRow* UEclipseInventoryItem::GetItemResourceData() const
{
	UEpGameDataSubsystem* GameDataSubSystem = GetGameDataSubsystem();
	if (ensure(GameDataSubSystem))
	{
		return GameDataSubSystem->GetGameData<FItemResourceDataRow>(ItemId);
	}

	return nullptr;
}

const FItemShapeDataRow* UEclipseInventoryItem::GetItemShapeData() const
{
	UEpGameDataSubsystem* GameDataSubsystem = GetGameDataSubsystem();
	if (ensure(GameDataSubsystem))
	{
		return GameDataSubsystem->GetGameData<FItemShapeDataRow>(ItemId);
	}

	return nullptr;
}

UEpGameDataSubsystem* UEclipseInventoryItem::GetGameDataSubsystem() const
{
	const UWorld* World = GetWorld();
	if (!ensure(World))
	{
		return nullptr;
	}

	UEpGameInstance* GameInst = Cast<UEpGameInstance>(World->GetGameInstance());
	if (!ensure(GameInst))
	{
		return nullptr;
	}

	return GameInst->GetSubsystem<UEpGameDataSubsystem>();
}