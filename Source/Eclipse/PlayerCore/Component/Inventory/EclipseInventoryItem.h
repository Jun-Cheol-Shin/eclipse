// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "EclipseInventoryItem.generated.h"


enum class EItemRarity : uint8;
enum class EClassType : uint8;

struct FItemDataRow;
struct FItemResourceDataRow;
struct FItemShapeDataRow;

class UEpGameDataSubSystem;

UCLASS()
class ECLIPSE_API UEclipseInventoryItem : public UObject
{
	GENERATED_BODY()
	
public:
	UEclipseInventoryItem() {}

	// Setter
	void SetItem(int32 InItemId, int64 InStackCount);
	void SetPosition(int32 InTopLeft);

	// Item Data
	FString						GetItemNameStr() const;
	EItemRarity					GetRarity() const;
	EClassType					GetClassTypeEnum() const;
	uint8						GetClassTypeBit() const;
	int64						GetMaxStackCount() const;

	// Item Resource Data
	FSoftObjectPath				GetMeshPath() const;
	FSoftObjectPath				GetThumbnailPath() const;

	// Item Shape Data
	// Hidden Index는 제외한 가로,세로 사이즈를 전달
	FIntPoint					GetItemSize() const;
	// Hidden Index를 제외한 실제 아이템 모양과 매칭시켜 인덱싱해 전달
	TArray<int32>				GetItemIndexList() const;


	FORCEINLINE int32			GetItemId() const { return ItemId; }
	FORCEINLINE int64			GetItemStackCount() const { return StackCount; }
	FORCEINLINE int32			GetPosition() const { return TopLeft; }

	bool						IsEqual(UEclipseInventoryItem* OtherItem);

private:
	const FItemDataRow*			GetItemData() const;
	const FItemResourceDataRow*	GetItemResourceData() const;
	const FItemShapeDataRow*	GetItemShapeData() const;
	UEpGameDataSubSystem*		GetGameDataSubSytem() const;

protected:
	virtual bool IsSupportedForNetworking() const override { return true; }
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	//int32						Serial;					// 임시 : Instance의 Unique id
	int32						ItemId = 0;
	int64						StackCount = 0;

	int32						TopLeft = INDEX_NONE;
};

