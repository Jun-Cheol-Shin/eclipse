// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "CommonInputTypeEnum.h"
#include "GuideMaskSettings.generated.h"

/**
 * 
 */

class UUserWidget;

UCLASS(Config = UIGuideMask)
class UIGUIDEMASKEDITOR_API UGuideMaskSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	TSoftClassPtr<UUserWidget> GetLayerClass() const { return GuideLayerWidgetClass; }
	int32 GetDefaultZOrder() const{ return DefaultZOrder; }
	ECommonInputType GetDefaultCommonInputType() const { return DefaultInputType; }
	
protected:
	virtual FName GetCategoryName() const override { return TEXT("Plugins"); } // "Project"로 바꾸고 싶으면 "Project"
	virtual FText GetSectionText() const override { return NSLOCTEXT("GuideMask", "Section", "GuideMask"); }

	
private:
	UPROPERTY(EditAnywhere, Config, Category = "UI", meta = (AllowAbstract = "false"))
	TSoftClassPtr<UUserWidget> GuideLayerWidgetClass = nullptr;
	
	UPROPERTY(EditAnywhere, Config, Category = "UI")
	int32 DefaultZOrder = 0;

	UPROPERTY(EditAnywhere, Config, Category = "UI")
	ECommonInputType DefaultInputType = ECommonInputType::MouseAndKeyboard;

};
