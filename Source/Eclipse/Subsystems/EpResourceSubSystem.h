// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/AssetManager.h"
#include "../EclipseTypes.h"
#include "Kismet/GameplayStatics.h"
#include "EpResourceSubSystem.generated.h"


#define EPResource(WorldContextObj)															\
([&]() -> UEpResourceSubSystem&                                                             \
{                                                                                           \
    UGameInstance* __GI = UGameplayStatics::GetGameInstance(WorldContextObj);               \
    check(__GI);                                                                            \
    UEpResourceSubSystem* __SS = __GI->GetSubsystem<UEpResourceSubSystem>();                \
    check(__SS);                                                                            \
    return *__SS;																			\
}())

#define EPColorPalette(WorldContextObj)														\
([&]() -> const UColorPaletteDataAsset&                                                     \
{                                                                                           \
    UGameInstance* __GI = UGameplayStatics::GetGameInstance(WorldContextObj);               \
    check(__GI);                                                                            \
    UEpResourceSubSystem* __SS = __GI->GetSubsystem<UEpResourceSubSystem>();                \
    check(__SS);																			\
	const UColorPaletteDataAsset* DataAsset = __SS->GetColorPalette();						\
	check(DataAsset);																		\
    return *DataAsset;																		\
}())

class UColorPaletteAsset;
class UObjectLibrary;
class AEpDropItemActor;

DECLARE_DELEGATE_OneParam(FOnLoadedFuncSignature, UObject*);

UCLASS()
class ECLIPSE_API UEpResourceSubSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

private:
	friend class UEpGameDataSubSystem;

public:
	// get blueprint class
	TSubclassOf<AEpDropItemActor> GetDropItemActor() const;


public:
	// get resource (data table, data asset..)
	UDataTable* GetDataTable(const FString& InDataName) const;
	const UColorPaletteDataAsset* GetColorPalette();

protected:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override { return true; }
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

private:
	TSharedPtr<FStreamableHandle> AsyncLoadObject(TWeakObjectPtr<UObject> OuterClass, const FAssetData& InAssetData, FOnLoadedFuncSignature OnLoadedDelegate) const;
	UObject* SyncLoadObject(const FAssetData& InAssetData) const;
	UClass* SyncLoadClass(const FAssetData& InAssetData, bool bUseGeneratedClass) const;

private:
	UPROPERTY()
	UColorPaletteDataAsset* ColorPalette = nullptr;

	UPROPERTY() 
	UObjectLibrary* AssetLibrary = nullptr;

	UPROPERTY() 
	TMap<FName, FAssetData> CachedAssetDataList;
};
