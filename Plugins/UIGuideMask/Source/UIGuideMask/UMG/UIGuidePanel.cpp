// Fill out your copyright notice in the Description page of Project Settings.


#include "UIGuidePanel.h"

#include "Components/PanelWidget.h"
#include "../UIGuideMaskable.h"
#include "../Subsystem/UIGuideMaskSubsystem.h"

#define LOCTEXT_NAMESPACE "UMG"

TSharedRef<SWidget> UUIGuidePanel::RebuildWidget()
{
	if (false == IsDesignTime())
	{
		UGameInstance* GameInstance = GetGameInstance();
		if (nullptr == GameInstance) return Super::RebuildWidget();

		UUIGuideMaskSubsystem* SubSystem = GameInstance->GetSubsystem<UUIGuideMaskSubsystem>();
		if (nullptr == SubSystem) return Super::RebuildWidget();;

		if (UUserWidget* OwnerUserWidget = GetTypedOuter<UUserWidget>())
		{
			if (OwnerUserWidget->GetClass()->ImplementsInterface(UUIGuideMaskable::StaticClass()))
			{
				TMap<FGameplayTag, UWidget*> Map = IUIGuideMaskable::Execute_OnGetMaskableWidget(OwnerUserWidget);

				for (auto& [Tag, Widget] : Map)
				{
					//SubSystem->SetGuideWidget(Tag, Widget);
				}
			}
		}
	}

	return Super::RebuildWidget();
}

void UUIGuidePanel::ReleaseSlateResources(bool bReleaseChildren)
{
	if (false == IsDesignTime())
	{
		UGameInstance* GameInstance = GetGameInstance();
		if (nullptr == GameInstance) return;

		UUIGuideMaskSubsystem* SubSystem = GameInstance->GetSubsystem<UUIGuideMaskSubsystem>();
		if (nullptr == SubSystem) return;

		if (UUserWidget* OwnerUserWidget = GetTypedOuter<UUserWidget>())
		{
			if (OwnerUserWidget->GetClass()->ImplementsInterface(UUIGuideMaskable::StaticClass()))
			{
				TMap<FGameplayTag, UWidget*> Map = IUIGuideMaskable::Execute_OnGetMaskableWidget(OwnerUserWidget);

				for (auto& [Tag, Widget] : Map)
				{
					//SubSystem->SetGuideWidget(Tag, Widget);
				}
			}
		}

	}

	Super::ReleaseSlateResources(bReleaseChildren);
}

void UUIGuidePanel::SynchronizeProperties()
{
	Super::SynchronizeProperties();

#if WITH_EDITOR
	if (UUserWidget* OwnerUserWidget = GetTypedOuter<UUserWidget>())
	{	
		if (OwnerUserWidget->GetClass()->ImplementsInterface(UUIGuideMaskable::StaticClass()))
		{
			TMap<FGameplayTag, UWidget*> Map = IUIGuideMaskable::Execute_OnGetMaskableWidget(OwnerUserWidget);

			RegistedTag.Reset();
			for (auto& [Tag, Widget] : Map)
			{
				RegistedTag.Add(Tag);
			}
		}
	}
#endif

}


#if WITH_EDITOR
TArray<FName> UUIGuidePanel::GetTagOptions() const
{
	TArray<FName> TagNameArray;

	for (int i = 0; i < RegistedTag.Num(); ++i)
	{
		TagNameArray.Add(RegistedTag[i].GetTagName());
	}

	return TagNameArray;
}

void UUIGuidePanel::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	FName PropertyName = PropertyChangedEvent.GetPropertyName();

	if (PropertyName == GET_MEMBER_NAME_CHECKED(UUIGuidePanel, TagName))
	{
		FGameplayTag FindTag;

		for (auto& Tag : RegistedTag)
		{
			if (Tag.GetTagName().IsEqual(TagName))
			{
				FindTag = Tag;
				break;
			}
		}

		UWidget* FindWidget = nullptr;

		UUserWidget* OwnerUserWidget = GetTypedOuter<UUserWidget>();
		if (!ensure(OwnerUserWidget)) return;


		if (true == OwnerUserWidget->GetClass()->ImplementsInterface(UUIGuideMaskable::StaticClass()))
		{
			TMap<FGameplayTag, UWidget*> Map = IUIGuideMaskable::Execute_OnGetMaskableWidget(OwnerUserWidget);

			if (Map.Contains(FindTag))
			{
				FindWidget = Map[FindTag];
			}
		}

		if (!ensure(FindWidget)) return;


		// todo

	}
}
const FText UUIGuidePanel::GetPaletteCategory()
{
	return LOCTEXT("UIGuideMask", "UIGuideMask");
}
#endif