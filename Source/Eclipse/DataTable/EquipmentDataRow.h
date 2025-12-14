// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EclipseRowBase.h"
#include "EquipmentDataRow.generated.h"


USTRUCT(Blueprintable)
struct FEquipmentDataRow : public FEclipseRowBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName AttachSocket;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FTransform AttachTransform;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FSoftObjectPath ActorToSpawn = FSoftObjectPath();
};
