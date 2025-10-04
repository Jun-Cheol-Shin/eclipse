// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ItemDataRow.generated.h"

UENUM(BlueprintType)
enum class EItemRarity : uint8
{
	Common = 0,
	Uncommon,
	Rare,
	Epic,
	Legendary,
	Unique,
};

UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EClassType : uint8
{
	None = 0		UMETA(Hidden),
	Knight = 1 << 1, 
	Assassin = 1 << 2,
	Berserker = 1 << 3
};


USTRUCT(Blueprintable)
struct FItemDataRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 ItemId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int64 MaxStackCount;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EItemRarity Rarity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Bitmask = "", BitmaskEnum = "/Script/Eclipse.EClassType"))
	uint8 Class = 1;
};