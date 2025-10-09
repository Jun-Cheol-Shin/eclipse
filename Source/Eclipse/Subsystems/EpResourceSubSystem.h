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
	const UColorPaletteDataAsset* DataAsset = __SS->GetPalette();							\
	check(DataAsset);																		\
    return *DataAsset;																		\
}())

class UColorPaletteAsset;

DECLARE_DELEGATE_OneParam(FOnLoadedFuncSignature, UObject*);

UCLASS()
class ECLIPSE_API UEpResourceSubSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

private:
	friend class UEpGameDataSubSystem;

public:
	template<typename T>
	TSharedPtr<FStreamableHandle> AsyncLoadObject(TSoftObjectPtr<T> InSoftPtr, TFunction<void(T*)> OnLoaded)
	{
		AsyncLoadObject(InSoftPtr, FOnLoadedFuncSignature::CreateWeakLambda(this, [OnLoaded](UObject* InObject)
			{
				OnLoaded(Cast<T>(InObject));
			}));
	}
	TSharedPtr<FStreamableHandle> AsyncLoadObject(TSoftObjectPtr<UObject> InSoftPtr, FOnLoadedFuncSignature OnLoadedDelegate);



	UFUNCTION(BlueprintPure)
	const UColorPaletteDataAsset* GetPalette() const { return ColorPalette; }

protected:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override { return true; }
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

private:
	bool GetDataTable(const FString& InDataName, OUT UDataTable** OutDataTable);

private:
	UPROPERTY()
	const UColorPaletteDataAsset* ColorPalette = nullptr;
};
