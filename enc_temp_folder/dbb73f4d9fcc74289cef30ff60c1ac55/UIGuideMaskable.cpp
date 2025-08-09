// Fill out your copyright notice in the Description page of Project Settings.


#include "UIGuideMaskable.h"


// Add default functionality here for any IUIGuideMaskable functions that are not pure virtual.

TMap<FGameplayTag, UWidget*> IUIGuideMaskable::OnGetMaskableWidget_Implementation() const
{
    return TMap<FGameplayTag, UWidget*>();
}

bool IUIGuideMaskable::IsCorrectListItem_Implementation(UObject* InItemClass) const
{
    return false;
}

bool IUIGuideMaskable::IsCorrectWidget_Implementation(UUserWidget* InWidget) const
{
    return false;
}
