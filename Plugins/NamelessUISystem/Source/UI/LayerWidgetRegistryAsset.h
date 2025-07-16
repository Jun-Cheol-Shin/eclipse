// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "LayerWidgetRegistryAsset.generated.h"


enum class EEclipseGameLayer : uint8;
class UCommonUserWidget;
class UEcpUserWidget;

USTRUCT(BlueprintType)
struct FSoftClassLayerWidgetAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	TArray<TSoftClassPtr<UCommonUserWidget>> WidgetSoftPtrList;
};

USTRUCT()
struct FRegistWidgetInfo
{
	GENERATED_BODY()

public:
	FRegistWidgetInfo() {};
	FRegistWidgetInfo(const FGameplayTag& InTag, const TSoftClassPtr<UCommonUserWidget>& InSoftPtr) : LayerTag(InTag), WidgetSoftPtr(InSoftPtr) {};
	virtual ~FRegistWidgetInfo() {};

public:
	FGameplayTag LayerTag;
	TSoftClassPtr<UCommonUserWidget> WidgetSoftPtr;
};


UCLASS(BlueprintType, MinimalAPI)
class ULayerWidgetRegistryAsset : public UDataAsset
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	TMap<FGameplayTag, FSoftClassLayerWidgetAsset> RegistryWidgets;

private:
	TMap<FString, FRegistWidgetInfo> ReverseRegistryList;

protected:
	virtual void PostLoad() override;

public:
	bool GetWidget(const FString& InPath, OUT FGameplayTag& OutLayerType, OUT TSoftClassPtr<UCommonUserWidget>* OutSoftPtr);
};
