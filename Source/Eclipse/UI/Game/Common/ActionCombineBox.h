// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "ActionCombineBox.generated.h"

/**
 * 
 */

class UInputAction;
class UDynamicEntryBox;

UCLASS()
class ECLIPSE_API UActionCombineBox : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	void SetAction(const UInputAction* InputAction);
	void SetCombinedAction(const UInputAction* InputAction, const UInputAction* InChordedAction);
	
protected:
	virtual void NativeConstruct() override;	

private:

	UPROPERTY(meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<UDynamicEntryBox> EntryBox = nullptr;
};
