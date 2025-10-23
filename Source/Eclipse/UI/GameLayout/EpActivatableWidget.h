// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "EpActivatableWidget.generated.h"

/**
 * 
 */

class UInputAction;
class UEnhancedInputComponent;
enum class ECommonInputType : uint8;

UCLASS()
class ECLIPSE_API UEpActivatableWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	const TArray<const UInputAction*>& GetRegistedInputActions() const { return InputActions; }

protected:
	virtual void OnShow() {};
	virtual void OnHide() {};
	virtual void OnChangedInputDevice(ECommonInputType InInputType) {};

private:
	virtual void NativeOnActivated() override;
	virtual void NativeOnDeactivated() override;
	virtual void SynchronizeProperties() override;

protected:
	UPROPERTY(VisibleDefaultsOnly, Category = "Eclipe Activatable Widget Settings")
	TArray<const UInputAction*> InputActions;

	TWeakObjectPtr<UEnhancedInputComponent> InputComponent;

private:
	//UPROPERTY(EditDefaultsOnly, Category = "Eclipse Activatable Widget Settings", meta = (AllowPrivateAccess = "true"))
	//TArray<FName> ActionKeys;
};
