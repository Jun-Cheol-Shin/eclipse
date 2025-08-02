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

protected:
	virtual bool IsEqual(int32 InGameDataId) { return false; };
	virtual bool IsEqual(const FString& InWidgetName) { return false; };
	virtual bool IsEqual(FGameplayTag InTag) { return false; };
};

