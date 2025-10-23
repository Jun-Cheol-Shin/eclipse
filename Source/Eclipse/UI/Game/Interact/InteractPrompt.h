// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../../GameLayout/EpActivatableWidget.h"
#include "InteractPromptEntry.h"
#include "Input/UIActionBindingHandle.h"
#include "InteractPrompt.generated.h"

/**
 * 
 */

class UInputAction;
class UCommonActionWidget;
class UDynamicEntryBox;
class UImage;
class UInteractPromptEntry;
class UWidgetSwitcher;
class AEpDropItemActor;

enum class ECommonInputType : uint8;

UCLASS()
class ECLIPSE_API UInteractPrompt : public UEpActivatableWidget
{
	GENERATED_BODY()
	
public:
	void Set(AEpDropItemActor* InActor);

protected:
	virtual void OnShow() override;
	virtual void OnHide() override;
	virtual void OnChangedInputDevice(ECommonInputType InType) override;

private:
	void SetGamepadUI();
	void SetKeyboardMouseUI();
	//void SetMobileUI();

protected:
	void OnInteract();
	void OnUseDirect();
	void OnPing();
	void OnHoldAction();

protected:
	virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override;

private:
	UPROPERTY(meta = (BindWidget, AllowPrivateAccess = "true"))
	UInteractPromptEntry* InteractEntry = nullptr;

	UPROPERTY(meta = (BindWidget, AllowPrivateAccess = "true"))
	UCommonTextBlock* HoldDisplayText = nullptr;

	UPROPERTY(meta = (BindWidget, AllowPrivateAccess = "true"))
	UImage* DisableImage = nullptr;

	UPROPERTY(meta = (BindWidget, AllowPrivateAccess = "true"))
	UCommonActionWidget* HoldActionWidget = nullptr;

	UPROPERTY(meta = (BindWidget, AllowPrivateAccess = "true"))
	UDynamicEntryBox* ExpandInteractBox = nullptr;
	
	UPROPERTY(meta = (BindWidget, AllowPrivateAccess = "true"))
	UWidgetSwitcher* ExpandSwitcher = nullptr;

private:
	TWeakObjectPtr<AEpDropItemActor> DropItemActor = nullptr;
	TArray<int32> ActionHandles;
};
