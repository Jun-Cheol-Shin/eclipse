// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "LayerWidgetRegistryAsset.generated.h"


enum class EEclipseGameLayer : uint8;
class UCommonActivatableWidget;

USTRUCT(BlueprintType)
struct FSoftClassLayerWidgetAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	TArray<TSoftClassPtr<UCommonActivatableWidget>> WidgetSoftPtrList;
};

USTRUCT()
struct FRegistWidgetInfo
{
	GENERATED_BODY()

public:
	FRegistWidgetInfo() {};
	FRegistWidgetInfo(const FGameplayTag& InTag, const TSoftClassPtr<UCommonActivatableWidget>& InSoftPtr) : LayerTag(InTag), WidgetSoftPtr(InSoftPtr) {};
	virtual ~FRegistWidgetInfo() {};

public:
	FGameplayTag LayerTag;
	TSoftClassPtr<UCommonActivatableWidget> WidgetSoftPtr;
};


UCLASS(BlueprintType, MinimalAPI)
class ULayerWidgetRegistryAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	//NAMELESSUISYSTEM_API void Initialize();
	NAMELESSUISYSTEM_API bool GetWidget(const FString& InPath, OUT FGameplayTag& OutLayerType, OUT TSoftClassPtr<UCommonActivatableWidget>* OutSoftPtr);

private:
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	TMap<FGameplayTag, FSoftClassLayerWidgetAsset> RegistryWidgets;

private:
	TMap<FString, FRegistWidgetInfo> ReverseRegistryList;

protected:
	virtual void PostLoad() override;

};
