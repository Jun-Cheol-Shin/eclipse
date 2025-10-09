// Fill out your copyright notice in the Description page of Project Settings.


#include "EpResourceSubSystem.h"
#include "AssetRegistry/AssetRegistryModule.h"

namespace ResourcePath
{
    const FName DataTablePath = TEXT("/Game/DataTable");
    const FName DataAssetPath = TEXT("/Game/DataAsset");
    const FName SpawnPath = TEXT("/Game/Blueprint/IngameActor");
}

TSharedPtr<FStreamableHandle> UEpResourceSubSystem::AsyncLoadObject(TSoftObjectPtr<UObject> InSoftPtr, FOnLoadedFuncSignature OnLoadedDelegate)
{
    if (UObject* Object = InSoftPtr.Get())
    {
        OnLoadedDelegate.ExecuteIfBound(Object);
        return nullptr;
    }

    FStreamableManager& StreamManager = UAssetManager::GetStreamableManager();
    return StreamManager.RequestAsyncLoad(InSoftPtr.ToSoftObjectPath(),
        FStreamableDelegate::CreateWeakLambda(this, [InSoftPtr, OnLoadedDelegate]()
            {
                OnLoadedDelegate.ExecuteIfBound(InSoftPtr.Get());
            }));
}

void UEpResourceSubSystem::Initialize(FSubsystemCollectionBase& Collection)
{
    FString Path = ResourcePath::DataAssetPath.ToString() + TEXT("/DA_ColorPalette");
    ColorPalette = Cast<UColorPaletteDataAsset>(FSoftObjectPath(Path).TryLoad());

    if (ensure(ColorPalette))
    {
        UE_LOG(LogTemp, Warning, TEXT("Success Load ColorPalette!"));
    }
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
