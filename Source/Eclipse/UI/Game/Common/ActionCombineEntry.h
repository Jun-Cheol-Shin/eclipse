// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "ActionCombineEntry.generated.h"

/**
 * 
 */

class UCommonTextBlock;
class UCommonActionWidget;
class UInputAction;

UCLASS()
class ECLIPSE_API UActionCombineEntry : public UCommonUserWidget
{
	GENERATED_BODY()
	
public:
	void SetKey(const FKey& Key);
	void SetEnableSign(bool bEnable);

protected:
	virtual void NativeConstruct() override;
	virtual void RemoveFromParent() override;
	
private:
	
	UPROPERTY(meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<UCommonTextBlock> PlusText = nullptr;

	UPROPERTY(meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<UCommonActionWidget> ActionWidget = nullptr;

};
