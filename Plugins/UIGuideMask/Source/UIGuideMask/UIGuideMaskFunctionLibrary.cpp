// Fill out your copyright notice in the Description page of Project Settings.


#include "UIGuideMaskFunctionLibrary.h"

#include "../UIGuideMask/Interface/UIGuideMaskable.h"
#include "../UIGuideMask/Subsystem/UIGuideMaskSubsystem.h"
#include "Blueprint/UserWidget.h"


#include "Components/WrapBox.h"
#include "Components/ListView.h"
#include "Components/DynamicEntryBox.h"


UWidget* UUIGuideMaskFunctionLibrary::GetWidget(UUserWidget* InOuterWidget, FGameplayTag InTag)
{
	if (nullptr == InOuterWidget) return nullptr;
	else if (false == InOuterWidget->GetClass()->ImplementsInterface(UUIGuideMaskable::StaticClass())) return nullptr;


	TMap<FGameplayTag, UWidget*> Map = IUIGuideMaskable::Execute_OnGetMaskableWidget(InOuterWidget);
	if (false == Map.Contains(InTag)) return nullptr;

	UWidget* TaggedWidget = Map[InTag];

	if (UListView* ListView = Cast<UListView>(TaggedWidget))
	{
		const TArray<UObject*>& ListItems = ListView->GetListItems();
		UUserWidget* FindEntry = nullptr;

#if WITH_EDITOR
		if (true == InOuterWidget->IsDesignTime())
		{
			if (true == ListItems.IsEmpty())
			{
				return TaggedWidget;
			}

			FindEntry = ListView->GetEntryWidgetFromItem(ListItems[0]);
			return nullptr == FindEntry ? TaggedWidget : FindEntry;
		}
#endif
		else
		{
			UObject* const* FoundItem = ListItems.FindByPredicate([&InOuterWidget](UObject* InItem)
				{
					return IUIGuideMaskable::Execute_IsCorrectListItem(InOuterWidget, InItem);
				});

			if (nullptr == FoundItem || nullptr == *FoundItem) return TaggedWidget;

			FindEntry = ListView->GetEntryWidgetFromItem(*FoundItem);
		}

		return nullptr == FindEntry ? TaggedWidget : FindEntry;
	}

	else if (UDynamicEntryBox* EntryBox = Cast<UDynamicEntryBox>(TaggedWidget))
	{
		const TArray<UUserWidget*>& Entries = EntryBox->GetAllEntries();

#if WITH_EDITOR
		if (true == InOuterWidget->IsDesignTime())
		{
			if (true == Entries.IsEmpty())
			{
				return TaggedWidget;
			}
			return nullptr == Entries[0] ? TaggedWidget : Entries[0];
		}
#endif
		else
		{
			UUserWidget* const* FoundEntry = Entries.FindByPredicate([&InOuterWidget](UUserWidget* InWidget)
				{
					return IUIGuideMaskable::Execute_IsCorrectEntryBoxWidget(InOuterWidget, InWidget);
				});


			return nullptr != FoundEntry && nullptr != *FoundEntry ? *FoundEntry : TaggedWidget;
		}
	}

	else if (UWrapBox* WrapBox = Cast<UWrapBox>(TaggedWidget))
	{
		const TArray<UWidget*>& Entries = WrapBox->GetAllChildren();

#if WITH_EDITOR
		if (true == InOuterWidget->IsDesignTime())
		{
			if (true == Entries.IsEmpty())
			{
				return TaggedWidget;
			}
			return nullptr == Entries[0] ? TaggedWidget : Entries[0];
		}
#endif
		else
		{
			UWidget* const* FoundEntry = Entries.FindByPredicate([&InOuterWidget](UWidget* InWidget)
				{
					return IUIGuideMaskable::Execute_IsCorrectWidget(InOuterWidget, InWidget);
				});


			return nullptr != FoundEntry && nullptr != *FoundEntry ? *FoundEntry : TaggedWidget;
		}
	}


	return TaggedWidget;
}

void UUIGuideMaskFunctionLibrary::ShowGuideWidget(const UGameInstance* InInstance, FGameplayTag InTag)
{
	UUIGuideMaskSubsystem* GuideSubSystem = InInstance->GetSubsystem<UUIGuideMaskSubsystem>();
	if (ensure(GuideSubSystem))
	{
		GuideSubSystem->ShowGuide(InInstance->GetFirstLocalPlayerController(), InTag);
	}
}