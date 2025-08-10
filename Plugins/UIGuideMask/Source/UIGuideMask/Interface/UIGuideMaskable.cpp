// Fill out your copyright notice in the Description page of Project Settings.


#include "UIGuideMaskable.h"


// Add default functionality here for any IUIGuideMaskable functions that are not pure virtual.

void IUIGuideMaskable::NativeOnStartGuide(FGameplayTag InGameplayTag, UWidget* InGuideWidget)
{
	Execute_OnStartGuide(Cast<UObject>(this), InGameplayTag, InGuideWidget);
}

void IUIGuideMaskable::NativeOnEndGuide(FGameplayTag InGameplayTag, UWidget* InGuideWidget)
{
	Execute_OnEndGuide(Cast<UObject>(this), InGameplayTag, InGuideWidget);
}

void IUIGuideMaskable::NativeOnCompleteAction(FGameplayTag InGameplayTag, UWidget* InGuideWidget)
{
	Execute_OnCompleteAction(Cast<UObject>(this), InGameplayTag, InGuideWidget);
}

TMap<FGameplayTag, UWidget*> IUIGuideMaskable::NativeOnGetMaskableWidget() const
{
	return Execute_OnGetMaskableWidget(Cast<UObject>(this));
}

bool IUIGuideMaskable::NativeIsCorrectListItem(UObject* InItemClass) const
{
	return Execute_IsCorrectListItem(Cast<UObject>(this), InItemClass);
}

bool IUIGuideMaskable::NativeIsCorrectEntryBoxWidget(UUserWidget* InWidget) const
{
	return Execute_IsCorrectEntryBoxWidget(Cast<UObject>(this), InWidget);
}

bool IUIGuideMaskable::NativeIsCorrectWidget(UWidget* InWidget) const
{
	return Execute_IsCorrectWidget(Cast<UObject>(this), InWidget);
}

void IUIGuideMaskable::NativeOnGetChildren(UObject* InParentItem, TArray<UObject*>& OutChildren) const
{
	return Execute_OnGetChildren(Cast<UObject>(this), InParentItem, OutChildren);
}

void IUIGuideMaskable::NativeOnCompletionPredicate(FGameplayTag InGameplayTag) const
{
	Execute_OnCompletionPredicate(Cast<UObject>(this), InGameplayTag);
}

bool IUIGuideMaskable::NativeOnEvaluateCompletionPredicate(FGameplayTag InGameplayTag) const
{
	return Execute_OnEvaluateCompletionPredicate(Cast<UObject>(this), InGameplayTag);
}

void IUIGuideMaskable::NativeOnFailedPredicate(FGameplayTag InGameplayTag) const
{
	return Execute_OnFailedPredicate(Cast<UObject>(this), InGameplayTag);
}
