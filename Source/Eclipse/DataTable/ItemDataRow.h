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
	int32 ItemId = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString ItemName = FString();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int64 MaxStackCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EItemRarity Rarity = EItemRarity::Common;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Bitmask = "", BitmaskEnum = "/Script/Eclipse.EClassType"))
	uint8 Class = 1;
};