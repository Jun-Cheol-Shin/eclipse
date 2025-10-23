// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "InteractPromptEntry.generated.h"

/**
 * 
 */

class UActionCombineBox;
class UCommonTextBlock;
class UInputAction;

USTRUCT(BlueprintType)
struct FInteractActionParam
{
	GENERATED_BODY()

public:
	const UInputAction* Action = nullptr;
	FText DisplayText = {};
};

UCLASS()
class ECLIPSE_API UInteractPromptEntry : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	void Set(const FInteractActionParam& InParam);

	//void SetCombinedAction(const FInteractActionParam& InParam);
	
	
private:
	UPROPERTY(meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<UCommonTextBlock> ActionDisplayText = nullptr;

	UPROPERTY(meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<UActionCombineBox> ActionCombineBox = nullptr;

};
