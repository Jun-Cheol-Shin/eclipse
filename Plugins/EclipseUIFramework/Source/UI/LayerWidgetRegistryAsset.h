// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LayerWidgetRegistryAsset.generated.h"


enum class EEclipseGameLayer : uint8;
class UEcpUserWidget;

USTRUCT(BlueprintType)
struct FSoftClassLayerWidgetAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	TArray<TSoftClassPtr<UEcpUserWidget>> WidgetSoftPtrList;
};


UCLASS(BlueprintType, MinimalAPI)
class ULayerWidgetRegistryAsset : public UDataAsset
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	TMap<EEclipseGameLayer, FSoftClassLayerWidgetAsset> RegistryWidgets;

private:
	TMap<FString, TPair<EEclipseGameLayer, TSoftClassPtr<UEcpUserWidget>>> ReverseRegistryList;

protected:
	virtual void PostLoad() override;

public:
	bool GetWidget(const FString& InPath, OUT EEclipseGameLayer& OutLayerType, OUT TSoftClassPtr<UEcpUserWidget>* OutSoftPtr);
};
