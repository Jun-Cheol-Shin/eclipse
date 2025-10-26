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
	DECLARE_MULTICAST_DELEGATE(FOnInteractSignature)
	FOnInteractSignature OnInteractDelegate;

	DECLARE_MULTICAST_DELEGATE(FOnPingSignature)
	FOnPingSignature OnPingDelegate;

	DECLARE_MULTICAST_DELEGATE(FOnUseItemSignature)
	FOnUseItemSignature OnUseDirectDelegate;


	void SetItem(int32 InItemId);

	// TODO
	//void SetInteractObject();


protected:
	virtual void OnShow() override;
	virtual void OnHide() override;
	virtual void OnChangedInputDevice(ECommonInputType InType) override;

private:
	void SetGamepadUI();
	void SetKeyboardMouseUI();
	//void SetMobileUI();

protected:
	UFUNCTION()
	void OnInteract();
	UFUNCTION()
	void OnUseDirect();
	UFUNCTION()
	void OnPing();
	UFUNCTION()
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
	UInteractPromptEntry* PingEntry = nullptr;

	UPROPERTY(meta = (BindWidget, AllowPrivateAccess = "true"))
	UInteractPromptEntry* UseDirectEntry = nullptr;
	
	UPROPERTY(meta = (BindWidget, AllowPrivateAccess = "true"))
	UWidgetSwitcher* ExpandSwitcher = nullptr;


private:
	int32 ItemId = 0;

};
