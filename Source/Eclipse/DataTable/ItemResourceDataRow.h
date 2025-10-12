// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ItemResourceDataRow.generated.h"

class UStaticMesh;
class UObject;

USTRUCT(Blueprintable)
struct FItemResourceDataRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 ItemId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FSoftObjectPath StaticMeshPath;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FSoftObjectPath ThumbnailImagePath;

};