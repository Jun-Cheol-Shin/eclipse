// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "UIGuideLayer.generated.h"

class UCanvasPanel;
class USizeBox;

UCLASS()
class UIGUIDEMASK_API UUIGuideLayer : public UCommonUserWidget
{
	GENERATED_BODY()

	friend class UUIGuideRegistrar;

private:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	UCanvasPanel* LayerPanel;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	USizeBox* GuideBoxPanel;



private:


	

	
};
