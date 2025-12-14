// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "../EclipseTypes.h"
#include "EclipseRowBase.generated.h"


USTRUCT(Blueprintable)
struct FEclipseRowBase : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Id = EclipseType::Invalid_GameDataId;

};