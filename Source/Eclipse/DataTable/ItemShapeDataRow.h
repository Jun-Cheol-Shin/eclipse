// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ItemShapeDataRow.generated.h"

/**
 * 
 */
USTRUCT(Blueprintable)
struct ECLIPSE_API FItemShapeDataRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 ItemId = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	uint8 Width = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	uint8 Height = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSet<uint8> HiddenIndexList = TSet<uint8>();
};
