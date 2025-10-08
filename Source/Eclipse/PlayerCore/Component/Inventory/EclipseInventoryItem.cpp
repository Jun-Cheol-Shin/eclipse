// Fill out your copyright notice in the Description page of Project Settings.


#include "EclipseInventoryItem.h"
#include "../../../GameModes/EpGameInstance.h"
#include "../../../Subsystems/EpGameDataSubSystem.h"

void UEclipseInventoryItem::SetItem(int32 InItemId, int64 InStackCount)
{
	ItemId = InItemId;
	StackCount = StackCount;
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

FSoftObjectPath UEclipseInventoryItem::GetIngameActorPath() const
{
	const FItemResourceDataRow* ItemData = GetItemResourceData();
	if (ItemData)
	{
		return ItemData->InGameActorPath.ToSoftObjectPath();
	}
	
	return FSoftObjectPath();
}

const FItemDataRow* UEclipseInventoryItem::GetItemData() const
{
	UEpGameDataSubSystem* GameDataSubSystem = GetGameDataSubSytem();
	if (ensure(GameDataSubSystem))
	{
		return GameDataSubSystem->GetGameData<FItemDataRow>(ItemId);
	}

	return nullptr;
}

const FItemResourceDataRow* UEclipseInventoryItem::GetItemResourceData() const
{
	UEpGameDataSubSystem* GameDataSubSystem = GetGameDataSubSytem();
	if (ensure(GameDataSubSystem))
	{
		return GameDataSubSystem->GetGameData<FItemResourceDataRow>(ItemId);
	}

	return nullptr;
}

UEpGameDataSubSystem* UEclipseInventoryItem::GetGameDataSubSytem() const
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

	return GameInst->GetSubsystem<UEpGameDataSubSystem>();
}
