// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "InputTriggers.h"
#include "EnhancedInputComponent.h"
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
	void BindUIAction(const FName& InActionAssetName, ETriggerEvent TriggerEvent, UObject* InObject, FName InFunctionName);
	void RemoveUIAction(const FName& InActionAssetName);


	const UInputAction* GetAction(const FName& InActionAssetName);

protected:
	virtual void OnShow() {};
	virtual void OnHide() {};
	virtual void OnDestroy() {};
	virtual void OnCreate() {};
	virtual void OnChangedInputDevice(ECommonInputType InInputType) {};
	virtual void OnBack() {};


private:
	virtual void NativeOnActivated() override;
	virtual void NativeOnDeactivated() override;
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void SynchronizeProperties() override;
	virtual bool NativeOnHandleBackAction() override;

protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Eclipe Activatable Widget Settings")
	TMap<FName, const UInputAction*> InputActions;

	TWeakObjectPtr<UEnhancedInputComponent> InputComponent;

private:
	TArray<FEnhancedInputActionEventBinding*> ActionHandles;
};
