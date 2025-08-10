// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayTagContainer.h"
#include "UIGuideMaskFunctionLibrary.generated.h"


class UUserWidget;
class UWidget;

UCLASS()
class UIGUIDEMASK_API UUIGuideMaskFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
	
public:
	UUIGuideMaskFunctionLibrary() {}

	// Access Guide SubSystem
	static void ShowGuideWidget(const UGameInstance* InInstance, FGameplayTag InTag);

	// ListView, DynamicEntryBox, WrapBox...
	static UWidget* GetWidget(UWidget* InOuterWidget, FGameplayTag InTag);

	static void ForEachWidgetRecursive(UWidget* Root, TFunctionRef<void(UWidget*)> Visit);
	
};
