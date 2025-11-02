// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Input/CommonBoundActionButton.h"
#include "EpTabListWidgetBase.h"
#include "EpBoundActionButton.generated.h"

class UCommonLazyImage;
class UCommonActionWidget;
class UCommonTextBlock;

UENUM(BlueprintType)
enum class EEpButtonState : uint8
{
	Disable = 0,
	Enable,
	Selected, 
};

UCLASS()
class ECLIPSE_API UEpBoundActionButton : public UCommonBoundActionButton, public IEclipseTabButtonInterface
{
	GENERATED_BODY()

	
protected:
	virtual void NativeOnSetTabInfo(const FEpTabParameter& TabDescriptor) override;
	virtual void SynchronizeProperties() override;

private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true", Category = "Button Display Setting"))
	EEpButtonState ButtonState;

private:
	UPROPERTY(meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<UCommonLazyImage> TabButtonIcon;
};
