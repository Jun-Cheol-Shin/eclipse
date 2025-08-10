// Fill out your copyright notice in the Description page of Project Settings.


#include "UIGuideMaskFunctionLibrary.h"

#include "../UIGuideMask/Interface/UIGuideMaskable.h"
#include "../UIGuideMask/Subsystem/UIGuideMaskSubsystem.h"
#include "Blueprint/UserWidget.h"

#include "Components/ContentWidget.h"
#include "Components/TreeView.h"
#include "Components/WrapBox.h"
#include "Components/ListView.h"
#include "Components/DynamicEntryBox.h"


void UUIGuideMaskFunctionLibrary::ShowGuideWidget(const UGameInstance* InInstance, FGameplayTag InTag)
{
	UUIGuideMaskSubsystem* GuideSubSystem = InInstance->GetSubsystem<UUIGuideMaskSubsystem>();
	if (ensure(GuideSubSystem))
	{
		GuideSubSystem->ShowGuide(InInstance->GetFirstLocalPlayerController(), InTag);
	}
}

UWidget* UUIGuideMaskFunctionLibrary::GetWidget(UWidget* InOuterWidget, FGameplayTag InTag)
{
	if (nullptr == InOuterWidget) return nullptr;
	else if (false == InOuterWidget->GetClass()->ImplementsInterface(UUIGuideMaskable::StaticClass())) return nullptr;


	TMap<FGameplayTag, UWidget*> Map = IUIGuideMaskable::Execute_OnGetMaskableWidget(InOuterWidget);
	if (false == Map.Contains(InTag)) return nullptr;

	UWidget* TaggedWidget = Map[InTag];

	if (UTreeView* TreeView = Cast<UTreeView>(TaggedWidget))
	{
		const TArray<UObject*>& ListItems = TreeView->GetListItems();
		UUserWidget* FindEntry = nullptr;

#if WITH_EDITOR
		if (true == InOuterWidget->IsDesignTime())
		{
			if (true == ListItems.IsEmpty())
			{
				return TaggedWidget;
			}

			FindEntry = TreeView->GetEntryWidgetFromItem(ListItems[0]);
			return nullptr == FindEntry ? TaggedWidget : FindEntry;
		}
#endif
		else
		{
			UObject* FindItem = nullptr;
			for (auto& Item : ListItems)
			{
				if (nullptr != FindItem) break;

				if (true == IUIGuideMaskable::Execute_IsCorrectListItem(InOuterWidget, Item))
				{
					FindItem = Item;
				}

				else
				{
					TArray<UObject*> ChildItems;
					IUIGuideMaskable::Execute_OnGetChildren(InOuterWidget, Item, OUT ChildItems);

					for (auto& ChildItem : ChildItems)
					{
						if (true == IUIGuideMaskable::Execute_IsCorrectListItem(InOuterWidget, ChildItem))
						{
							FindItem = ChildItem;
						}
					}
				}
			}


			FindEntry = TreeView->GetEntryWidgetFromItem(FindItem);
		}

		return nullptr == FindEntry ? TaggedWidget : FindEntry;
	}

	else if (UListView* ListView = Cast<UListView>(TaggedWidget))
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

void UUIGuideMaskFunctionLibrary::ForEachWidgetRecursive(UWidget* Root, TFunctionRef<void(UWidget*)> Visit)
{
	if (!Root) return;


	Visit(Root);

	if (auto* Panel = Cast<UPanelWidget>(Root))
	{
		const int32 Count = Panel->GetChildrenCount();
		for (int32 i = 0; i < Count; ++i)
		{
			ForEachWidgetRecursive(Panel->GetChildAt(i), Visit);
		}
	}
	else if (auto* Content = Cast<UContentWidget>(Root))
	{
		ForEachWidgetRecursive(Content->GetContent(), Visit);
	}
	else if (auto* AsUser = Cast<UUserWidget>(Root))
	{
		ForEachWidgetRecursive(AsUser->GetRootWidget(), Visit);
	}
}