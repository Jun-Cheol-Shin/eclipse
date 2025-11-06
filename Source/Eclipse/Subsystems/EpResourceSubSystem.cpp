// Fill out your copyright notice in the Description page of Project Settings.


#include "EpResourceSubSystem.h"
#include "EpGameDataSubSystem.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "UObject/TopLevelAssetPath.h" 
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
            ResourcePath::DataTablePath,
            ResourcePath::DataAssetPath,
            ResourcePath::SpawnPath,
            ResourcePath::StaticMeshPath,
        }
        ));

    AssetLibrary->LoadAssetsFromAssetData();
    TArray<FAssetData> DataList;
    AssetLibrary->GetAssetDataList(DataList);

    Algo::Transform(DataList, CachedAssetDataList, [](const FAssetData& InAssetData) -> TPair<FTopLevelAssetPath, FAssetData>
        {
            if (InAssetData.IsInstanceOf(UDataTable::StaticClass()))
            {
                const FName TagName("RowStructure");
                FAssetTagValueRef TagValue = InAssetData.TagsAndValues.FindTag(TagName);
                FSoftObjectPath SoftPath = TagValue.GetValue();

                return { SoftPath.GetAssetPath(), InAssetData };
            }

            return { InAssetData.AssetClassPath, InAssetData };
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
        FTopLevelAssetPath ClassPath = UColorPaletteDataAsset::StaticClass()->GetClassPathName();

        if (CachedAssetDataList.Contains(ClassPath))
        {
            ColorPalette = Cast<UColorPaletteDataAsset>(SyncLoadObject(CachedAssetDataList.FindRef(ClassPath), true));
        }

        else
        {
            UE_LOG(LogTemp, Error, TEXT("Invalid Resource.. %s"), ANSI_TO_TCHAR(__FUNCTION__));
        }
    }

    return ColorPalette;
}

TSharedPtr<FStreamableHandle> UEpResourceSubSystem::AsyncLoadObject(TWeakObjectPtr<UObject> OuterClass, const FAssetData& InAssetData, FOnLoadedFuncSignature OnLoadedDelegate) const
{
    FStreamableManager& StreamManager = UAssetManager::GetStreamableManager();

    FSoftObjectPath Path;
    FString OutValue;

    if (true == InAssetData.GetTagValue(TEXT("GeneratedClass"), OUT OutValue))
    {
        Path = FSoftObjectPath(OutValue);
    }
    else
    {
        Path = InAssetData.ToSoftObjectPath();
    }

    TSharedPtr<FStreamableHandle> Handle = StreamManager.RequestAsyncLoad(Path,
        FStreamableDelegate::CreateWeakLambda(this, [OuterClass, OnLoadedDelegate, InAssetData]()
            {
                if (ensure(OuterClass.IsValid()))
                {
                    OnLoadedDelegate.ExecuteIfBound(InAssetData.GetAsset());
                }
            }));

    return Handle;
}

UObject* UEpResourceSubSystem::SyncLoadObject(const FAssetData& InAssetData, bool bUseGeneratedClass) const
{
    FStreamableManager& StreamManager = UAssetManager::GetStreamableManager();
    FString OutValue;

    if (true == bUseGeneratedClass && true == InAssetData.GetTagValue(TEXT("GeneratedClass"), OUT OutValue))
    {
        FSoftObjectPath Path = FSoftObjectPath(OutValue);
        return StreamManager.LoadSynchronous(Path);
    }

    return StreamManager.LoadSynchronous(InAssetData.ToSoftObjectPath());
}
