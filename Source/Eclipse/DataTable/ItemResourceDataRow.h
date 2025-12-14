// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EclipseRowBase.h"
#include "ItemResourceDataRow.generated.h"

class UStaticMesh;
class UObject;

USTRUCT(Blueprintable)
struct FItemResourceDataRow : public FEclipseRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FSoftObjectPath StaticMeshPath = FSoftObjectPath();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FSoftObjectPath ThumbnailImagePath = FSoftObjectPath();

};