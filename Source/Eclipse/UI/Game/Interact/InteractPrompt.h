// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "InteractPromptEntry.h"
#include "InteractPrompt.generated.h"

/**
 * 
 */

class UInputAction;
class UDynamicEntryBox;

UCLASS()
class ECLIPSE_API UInteractPrompt : public UCommonActivatableWidget
{
	GENERATED_BODY()
	
public:
	void Set(const TArray<FInteractActionParam>& InParams);

protected:
	virtual void NativeOnActivated() override;
	virtual void NativeOnDeactivated() override;
	
	
private:
	UPROPERTY(meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<UDynamicEntryBox> EntryBox = nullptr;
};
