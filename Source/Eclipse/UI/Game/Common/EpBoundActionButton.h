// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Input/CommonBoundActionButton.h"
#include "EpTabListWidgetBase.h"
#include "EpBoundActionButton.generated.h"

class UCommonLazyImage;
class UCommonActionWidget;
class UCommonTextBlock;

UCLASS()
class ECLIPSE_API UEpBoundActionButton : public UCommonBoundActionButton, public IEclipseTabButtonInterface
{
	GENERATED_BODY()

public:
	void SetButtonState(EEpButtonState InState);
	
protected:
	// CommonButton Func
	virtual void NativeOnClicked() override;
	// End CommonButton Func

	// IEclipseTabButtonInterface
	virtual void NativeOnChangedTabState(EEpButtonState InState) override;
	virtual void NativeOnSetTabInfo(const FEpTabParameter& TabDescriptor) override;
	// End IEclipseTabButtonInterface

	virtual void SynchronizeProperties() override;

private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true", Category = "Button Display Setting"))
	EEpButtonState ButtonState;

private:
	UPROPERTY(meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<UCommonLazyImage> TabButtonIcon;
};
