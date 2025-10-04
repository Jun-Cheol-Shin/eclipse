// Fill out your copyright notice in the Description page of Project Settings.


#include "EpResourceSubSystem.h"
#include "AssetRegistry/AssetRegistryModule.h"

namespace ResourcePath
{
    const FName DataTablePath = TEXT("/Game/DataTables");
    const FName SpawnPath = TEXT("/Game/Blueprint/Spawn");
}

void UEpResourceSubSystem::Initialize(FSubsystemCollectionBase& Collection)
{

}

void UEpResourceSubSystem::Deinitialize()
{

}

bool UEpResourceSubSystem::GetDataTable(const FString& InDataName, OUT UDataTable** OutDataTable)
{
    FString Path = ResourcePath::DataTablePath.ToString() + TEXT("/DT_") + InDataName;
    *OutDataTable = Cast<UDataTable>(FSoftObjectPath(Path).TryLoad());

    if (nullptr != OutDataTable)
    {
        return true;
    }

    return false;
}
