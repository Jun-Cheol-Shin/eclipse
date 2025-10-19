// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "NLUISystemSettings.generated.h"

class UNLGameLayout;
class ULayerWidgetRegistryAsset;

/**
 * 
 */
UCLASS(Config = Game, DefaultConfig)
class UNLUISystemSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
private:
	friend class UNLUIManagerSubsystem;

	UPROPERTY(EditAnywhere, Config, meta = (AllowedClasses = "/Script/NamelessUISystem.NLGameLayout"))
	TSoftClassPtr<UNLGameLayout> LayoutClass;

	UPROPERTY(EditAnywhere, Config, meta = (AllowedClasses = "/Script/NamelessUISystem.LayerWidgetRegistryAsset"))
	TSoftObjectPtr<ULayerWidgetRegistryAsset> WidgetRegistryAssetSoftPtr;
	
	// 섹션 이름 고정(모듈 바뀌어도 동일 섹션)
	virtual FName GetContainerName() const override { return TEXT("Project"); }
	virtual FName GetCategoryName()  const override { return TEXT("Plugins"); }
	virtual FName GetSectionName()   const override { return TEXT("NLUISystem Settings"); }

};
