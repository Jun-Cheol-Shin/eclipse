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
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "UI Guide Helper Function")
	static void ShowGuideWidget(const UGameInstance* InInstance, FGameplayTag InTag);

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "UI Guide Helper Function")
	static void ShowGuideWidgetSteps(const UGameInstance* InInstance, const TArray<FGameplayTag>& InTagList);



	// ListView, DynamicEntryBox, WrapBox...
	static UWidget* GetTagWidget(UWidget* InOuterWidget, FGameplayTag InTag);
	static UWidget* GetEntry(UWidget* InOuterWidget, UWidget* InTagetWidget);
	static bool IsContainerWidget(UWidget* InTargetWidget);


	static void ForEachWidgetRecursive(UWidget* Root, TFunctionRef<void(UWidget*)> Visit);
	
};
