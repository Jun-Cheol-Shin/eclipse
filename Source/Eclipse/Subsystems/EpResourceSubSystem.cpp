// Fill out your copyright notice in the Description page of Project Settings.


#include "EpResourceSubSystem.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/ObjectLibrary.h"

namespace ResourcePath
{
    const FString DataTablePath = TEXT("/Game/Resource/DataTable");
    const FString DataAssetPath = TEXT("/Game/Resource/DataAsset");
    const FString SpawnPath = TEXT("/Game/Resource/Blueprint/IngameActor");
    const FString MaterialPath = TEXT("/Game/Resource/Material");
    const FString StaticMeshPath = TEXT("/Game/Resource/Mesh");
}

void UEpResourceSubSystem::Initialize(FSubsystemCollectionBase& Collection)
{
    AssetLibrary = UObjectLibrary::CreateLibrary(UObject::StaticClass(), true, GIsEditor);
    AssetLibrary->LoadAssetDataFromPaths(TArray<FString>(
        {
            ResourcePath::DataAssetPath,
            ResourcePath::DataTablePath,
            ResourcePath::SpawnPath,
            ResourcePath::MaterialPath,
            ResourcePath::StaticMeshPath,
        }
        ));

    AssetLibrary->LoadAssetsFromAssetData();

    TArray<FAssetData> DataList;
    AssetLibrary->GetAssetDataList(DataList);

    Algo::Transform(DataList, CachedAssetDataList, [](const FAssetData& InAssetData) -> TPair<FName, FAssetData>
        {
            return { InAssetData.AssetName, InAssetData };
        });
}

void UEpResourceSubSystem::Deinitialize()
{
    CachedAssetDataList.Reset();
    ColorPalette = nullptr;
}

const UColorPaletteDataAsset* UEpResourceSubSystem::GetColorPalette()
{
    if (nullptr == ColorPalette)
    {
        FName ColorPaletteName = FName(TEXT("DA_ColorPalette"));

        if (CachedAssetDataList.Contains(ColorPaletteName))
        {
            ColorPalette = Cast<UColorPaletteDataAsset>(SyncLoadObject(CachedAssetDataList.FindRef(ColorPaletteName)));
        }
    }

    return ColorPalette;
}

bool UEpResourceSubSystem::GetDataTable(const FString& InDataName, OUT UDataTable** OutDataTable) const
{
    FName DataTableName = FName(TEXT("DT_") + InDataName);

    if (CachedAssetDataList.Contains(DataTableName))
    {
        *OutDataTable = Cast<UDataTable>(SyncLoadObject(CachedAssetDataList.FindRef(DataTableName)));
        return true;
    }

    return false;
}


TSharedPtr<FStreamableHandle> UEpResourceSubSystem::AsyncLoadObject(TWeakObjectPtr<UObject> OuterClass, const FAssetData& InAssetData, FOnLoadedFuncSignature OnLoadedDelegate)
{
    FStreamableManager& StreamManager = UAssetManager::GetStreamableManager();

    TSharedPtr<FStreamableHandle> Handle = StreamManager.RequestAsyncLoad(InAssetData.ToSoftObjectPath(),
        FStreamableDelegate::CreateWeakLambda(this, [OuterClass, OnLoadedDelegate, InAssetData]()
            {
                if (ensure(OuterClass.IsValid()))
                {
                    OnLoadedDelegate.ExecuteIfBound(InAssetData.GetAsset());
                }
            }));

    return Handle;
}

UObject* UEpResourceSubSystem::SyncLoadObject(const FAssetData& InAssetData) const
{
    FStreamableManager& StreamManager = UAssetManager::GetStreamableManager();
    return StreamManager.LoadSynchronous(InAssetData.ToSoftObjectPath());
}

UClass* UEpResourceSubSystem::SyncLoadClass(const FAssetData& InAssetData, bool bUseGeneratedClass) const
{
    FStreamableManager& StreamManager = UAssetManager::GetStreamableManager();
    FString OutValue;

    if (true == bUseGeneratedClass && true == InAssetData.GetTagValue(TEXT("GeneratedClass"), OUT OutValue))
    {
        UObject* LoadedClass = StreamManager.LoadSynchronous(OutValue);
        if (LoadedClass)
        {
            return LoadedClass->GetClass()->GetDefaultObject()->StaticClass();
        }
    }

    else
    {
        UObject* LoadedClass = StreamManager.LoadSynchronous(InAssetData.ToSoftObjectPath());
        if (LoadedClass)
        {
            return LoadedClass->GetClass()->GetDefaultObject()->StaticClass();
        }
    }

    return nullptr;
}

