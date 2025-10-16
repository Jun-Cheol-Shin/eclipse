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
UCLASS()
class UNLUISystemSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
private:
	friend class UNLUIManagerSubsystem;

	UPROPERTY(EditAnywhere, meta = (AllowedClasses = "/Script/NamelessUISystem.NLGameLayout"))
	TSoftClassPtr<UNLGameLayout> LayoutClass;

	UPROPERTY(EditAnywhere, meta = (AllowedClasses = "/Script/NamelessUISystem.LayerWidgetRegistryAsset"))
	TSoftObjectPtr<ULayerWidgetRegistryAsset> WidgetRegistryAssetSoftPtr;
	
	// 섹션 이름 고정(모듈 바뀌어도 동일 섹션)
	virtual FName GetContainerName() const override { return TEXT("Project"); }
	virtual FName GetCategoryName()  const override { return TEXT("Plugins"); }
	virtual FName GetSectionName()   const override { return TEXT("Nameless UI System Setting"); }

};
