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
	friend class UUIGuideMaskSubsystem;

	virtual void NativeOnStartGuide(FGameplayTag InGameplayTag, UWidget* InGuideWidget);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Guide Maskable", DisplayName = "On Start Guide")
	void OnStartGuide(FGameplayTag InGameplayTag, UWidget* InGuideWidget);


	virtual void NativeOnEndGuide(FGameplayTag InGameplayTag, UWidget* InGuideWidget);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Guide Maskable", DisplayName = "On End Guide")
	void OnEndGuide(FGameplayTag InGameplayTag, UWidget* InGuideWidget);


	virtual void NativeOnCompleteAction(FGameplayTag InGameplayTag, UWidget* InGuideWidget);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Guide Maskable", DisplayName = "On Complete Action")
	void OnCompleteAction(FGameplayTag InGameplayTag, UWidget* InGuideWidget);


protected:
	virtual TMap<FGameplayTag, UWidget*> NativeOnGetMaskableWidget() const;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Guide Maskable", DisplayName = "On Get Regist Guide Target Widget")
	TMap<FGameplayTag, UWidget*> OnGetMaskableWidget() const;


	virtual bool NativeIsCorrectListItem(UObject* InItemClass) const;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Guide Maskable", DisplayName = "On Get Is Correct List Item")
	bool IsCorrectListItem(UObject* InItemClass) const;



	virtual bool NativeIsCorrectEntryBoxWidget(UUserWidget* InWidget) const;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Guide Maskable", DisplayName = "On Get Is Correct Entry Widget")
	bool IsCorrectEntryBoxWidget(UUserWidget* InWidget) const;
	


	virtual bool NativeIsCorrectWidget(UWidget* InWidget) const;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Guide Maskable", DisplayName = "On Get Is Correct Widget")
	bool IsCorrectWidget(UWidget* InWidget) const;



	virtual void NativeOnGetChildren(UObject* InParentItem, TArray<UObject*>& OutChildren) const;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Guide Maskable", DisplayName = "On Get Treeview Childrens")
	void OnGetChildren(UObject* InParentItem, TArray<UObject*>& OutChildren) const;


	virtual void NativeOnCompletionPredicate(FGameplayTag InGameplayTag) const;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Guide Maskable", DisplayName = "On Called Completion Predicate")
	void OnCompletionPredicate(FGameplayTag InGameplayTag) const;



	virtual bool NativeOnEvaluateCompletionPredicate(FGameplayTag InGameplayTag) const;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Guide Maskable", DisplayName = "On Evaluate Completion Predicate")
	bool OnEvaluateCompletionPredicate(FGameplayTag InGameplayTag) const;


	virtual void NativeOnFailedPredicate(FGameplayTag InGameplayTag) const;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Guide Maskable", DisplayName = "On Failed Predicate")
	void OnFailedPredicate(FGameplayTag InGameplayTag) const;
};

