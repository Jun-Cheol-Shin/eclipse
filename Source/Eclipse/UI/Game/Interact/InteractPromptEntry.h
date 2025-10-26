// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "InteractPromptEntry.generated.h"

/**
 * 
 */
class UInputMappingContext;
class UActionCombineBox;
class UCommonTextBlock;
class UInputAction;

USTRUCT(BlueprintType)
struct FInteractActionParam
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	const UInputMappingContext* Context = nullptr;

	UPROPERTY(EditAnywhere)
	const UInputAction* Action = nullptr;

	UPROPERTY(EditAnywhere)
	FText DisplayText = {};
};

UCLASS()
class ECLIPSE_API UInteractPromptEntry : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	void Set(const FInteractActionParam& InParam);
	void Refresh();

private:
	bool IsCorrectInputType(FKey InKey) const;

private:
	UPROPERTY(meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<UCommonTextBlock> ActionDisplayText = nullptr;

	UPROPERTY(meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<UActionCombineBox> ActionCombineBox = nullptr;

protected:
	UPROPERTY(EditInstanceOnly, meta = (Category = "Setting"))
	FInteractActionParam Parameter;
};
