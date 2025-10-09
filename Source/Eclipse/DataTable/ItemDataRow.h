// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "../EclipseTypes.h"
#include "ItemDataRow.generated.h"

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