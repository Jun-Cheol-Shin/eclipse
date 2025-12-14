// Fill out your copyright notice in the Description page of Project Settings.


#include "EpGameDataSubSystem.h"

// Data Row
#include "../DataTable/ItemDataRow.h"
#include "../DataTable/ItemResourceDataRow.h"
#include "../DataTable/ItemShapeDataRow.h"

void UEpGameDataSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	UEpResourceSubsystem* ResourceManager = Collection.InitializeDependency<UEpResourceSubsystem>();

	if (nullptr != ResourceManager)
	{
		AddGameData<FItemDataRow>(ResourceManager);
		AddGameData<FItemShapeDataRow>(ResourceManager);
		//AddGameData<FItemResourceDataRow>(ResourceManager);
	}
}

void UEpGameDataSubsystem::Deinitialize()
{
	DataTables.Reset();
}
