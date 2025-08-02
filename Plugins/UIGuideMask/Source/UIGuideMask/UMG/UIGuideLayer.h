// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "UIGuideTooltip.h"
#include "UIGuideLayer.generated.h"

class UCanvasPanel;
class USizeBox;
class UUIGuideTooltip;
class UImage;

UCLASS()
class UIGUIDEMASK_API UUIGuideLayer : public UCommonUserWidget
{
	GENERATED_BODY()

	friend class UUIGuideRegistrar;

public:
	void Set(const FGeometry& InGeometry, const UWidget* InWidget, const FGuideMessageParameters& InMessageParam = FGuideMessageParameters());


private:
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	ETooltipPosition TooltipPosition;

private:
	void OnResizedViewport(FViewport* InViewport, uint32 InMessage);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;


private:
	UPROPERTY(meta = (BindWidget, AllowPrivateAccess = "true"))
	UCanvasPanel* LayerPanel;

	UPROPERTY(meta = (BindWidget, AllowPrivateAccess = "true"))
	UImage* BlackScreen;

	UPROPERTY(meta = (BindWidget, AllowPrivateAccess = "true"))
	USizeBox* GuideBoxPanel;

	UPROPERTY(meta = (BindWidget, AllowPrivateAccess= "true"))
	UUIGuideTooltip* GuideTooltip;	

	
};
