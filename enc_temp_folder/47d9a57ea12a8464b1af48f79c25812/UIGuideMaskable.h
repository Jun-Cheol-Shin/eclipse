// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Components/Widget.h"
#include "GameplayTagContainer.h"

#include "UIGuideMaskable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, Blueprintable)
class UUIGuideMaskable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class UIGUIDEMASK_API IUIGuideMaskable
{
	GENERATED_BODY()

protected:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Guide Maskable", DisplayName = "Regist Guide Target Widget")
	TMap<FGameplayTag, UWidget*> OnGetMaskableWidget() const;
	virtual TMap<FGameplayTag, UWidget*> OnGetMaskableWidget_Implementation() const;


	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Guide Maskable", DisplayName = "Check List Item")
	bool IsCorrectListItem(UObject* InItemClass) const;
	virtual bool IsCorrectListItem_Implementation(UObject* InItemClass) const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Guide Maskable", DisplayName = "Check Widget")
	bool IsCorrectWidget(UUserWidget* InWidget) const;
	virtual bool IsCorrectWidget_Implementation(UUserWidget* InWidget) const;
};

