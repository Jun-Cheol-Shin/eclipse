// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "EclipseInventoryItem.generated.h"


enum class EItemRarity : uint8;
enum class EClassType : uint8;

struct FItemDataRow;
struct FItemResourceDataRow;

class UEpGameDataSubSystem;

UCLASS()
class ECLIPSE_API UEclipseInventoryItem : public UObject
{
	GENERATED_BODY()
	
public:
	UEclipseInventoryItem() {}

	// Setter
	void SetItem(int32 InItemId, int64 InStackCount);

	// Item Data
	FString						GetItemNameStr() const;
	EItemRarity					GetRarity() const;
	EClassType					GetClassTypeEnum() const;
	uint8						GetClassTypeBit() const;
	int64						GetMaxStackCount() const;

	// Item Resource Data
	FSoftObjectPath				GetMeshPath() const;
	FSoftObjectPath				GetThumbnailPath() const;

	// TODO : 아이템 사이즈에 대한 데이터 필요
	const FIntPoint&			GetItemSize() const { return FIntPoint(0, 0); }


	FORCEINLINE int32			GetItemId() const { return ItemId; }
	FORCEINLINE int64			GetItemStackCount() const { return StackCount; }

	bool						IsEqual(UEclipseInventoryItem* OtherItem);

	const FIntPoint&			GetTopLeft() const;

private:
	const FItemDataRow*			GetItemData() const;
	const FItemResourceDataRow*	GetItemResourceData() const;
	UEpGameDataSubSystem*		GetGameDataSubSytem() const;

protected:
	virtual bool IsSupportedForNetworking() const override { return true; }
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	FGuid						Serial;
	int32						ItemId = 0;
	int64						StackCount = 0;
	
	// Grid System
	FIntPoint					TopLeft = FIntPoint(-1, -1); // X, Y

};

