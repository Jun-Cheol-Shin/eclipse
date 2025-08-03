// Fill out your copyright notice in the Description page of Project Settings.


#include "UIGuideRegistrar.h"
#include "UIGuideLayer.h"

#include "Components/OverlaySlot.h"
#include "Components/Overlay.h"
#include "Components/NamedSlot.h"
#include "Components/PanelWidget.h"

#include "../UIGuideMaskable.h"
#include "../Subsystem/UIGuideMaskSubsystem.h"

#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"


#define LOCTEXT_NAMESPACE "UMG"



#if WITH_EDITOR

TArray<FName> UUIGuideRegistrar::GetTagOptions() const
{
	TArray<FName> TagNameArray;

	for (int i = 0; i < RegistedTag.Num(); ++i)
	{
		TagNameArray.Add(RegistedTag[i].GetTagName());
	}

	return TagNameArray;
}

void UUIGuideRegistrar::ShowPreviewDebug()
{
	HidePreviewDebug();

	ForceLayoutPrepass();

	if (PreviewGuideLayer.IsValid())
	{
		CreatePreviewLayer();
	}


	else if (PreviewGuideLayer.ToSoftObjectPath().IsValid())
	{
		FStreamableManager& StreamableManager = UAssetManager::Get().GetStreamableManager();
		TSharedPtr<FStreamableHandle> StreamingHandle = StreamableManager.RequestAsyncLoad(PreviewGuideLayer.ToSoftObjectPath(), FStreamableDelegate::CreateWeakLambda(this,
			[this]()
			{
				CreatePreviewLayer();

			}));
	}

}
void UUIGuideRegistrar::HidePreviewDebug()
{
	for (int i = GuideOverlay->GetChildrenCount() - 1; i >= 2 ; --i)
	{
		UWidget* Child = GuideOverlay->GetChildAt(i);
		if (Child) Child->RemoveFromParent();
	}

	WidgetPool.ReleaseAll();
}

void UUIGuideRegistrar::CreatePreviewLayer()
{
	UWidget* TaggedWidget = nullptr;
	if (false == GetTaggedWidget(&TaggedWidget))
	{
		return;
	}

	UUIGuideLayer* PreviewGuideLayerWidget = WidgetPool.GetOrCreateInstance<UUIGuideLayer>(PreviewGuideLayer.Get());
	if (PreviewGuideLayerWidget)
	{
		FGameplayTag SelectedTag = GetTag(PreviewWidgetTag);

		FGuideParameter Parameter;
		if (TextParameters.Contains(SelectedTag))
		{
			Parameter = TextParameters[SelectedTag];
		}

		if (UOverlaySlot* OverlaySlot = GuideOverlay->AddChildToOverlay(PreviewGuideLayerWidget))
		{
			OverlaySlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
			OverlaySlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
		}

		PreviewGuideLayerWidget->Set(NamedSlot->GetCachedGeometry(), TaggedWidget, Parameter);

	}
}

bool UUIGuideRegistrar::GetTaggedWidget(OUT UWidget** OutWidget)
{
	UUserWidget* OwnerUserWidget = GetTypedOuter<UUserWidget>();
	if (nullptr == OwnerUserWidget || this == OwnerUserWidget) return false;


	if (true == OwnerUserWidget->GetClass()->ImplementsInterface(UUIGuideMaskable::StaticClass()))
	{
		TMap<FGameplayTag, UWidget*> Map = IUIGuideMaskable::Execute_OnGetMaskableWidget(OwnerUserWidget);

		FGameplayTag SelectedTag = GetTag(PreviewWidgetTag);

		if (Map.Contains(SelectedTag))
		{
			*OutWidget = Map[SelectedTag];

			return true;
		}
	}

	return false;
}
FGameplayTag UUIGuideRegistrar::GetTag(const FName& InTagName)
{
	for (auto& Tag : RegistedTag)
	{
		if (Tag.GetTagName().IsEqual(InTagName))
		{
			return Tag;
		}
	}

	return FGameplayTag();
}
#endif


void UUIGuideRegistrar::NativeConstruct()
{
	Super::NativeConstruct();

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
					FGuideData NewData;
					NewData.GameplayTag = Tag;
					NewData.TargetWidget = Widget;

					FGuideParameter NewParameter;
					if (true == TextParameters.Contains(Tag))
					{
						NewParameter = TextParameters[Tag];
					}

					NewData.GuideParameters = NewParameter;
					SubSystem->RegistGuideWidget(NewData);
				}
			}
		}

		UOverlaySlot* OverlaySlot = Cast<UOverlaySlot>(NamedSlot->Slot);
		if (OverlaySlot)
		{
			UWidget* NamedSlotWidget = NamedSlot->GetChildAt(0);
			UCanvasPanel* CanvasPanel = Cast<UCanvasPanel>(NamedSlotWidget);

			OverlaySlot->SetHorizontalAlignment(nullptr != CanvasPanel ? EHorizontalAlignment::HAlign_Fill : EHorizontalAlignment::HAlign_Center);
			OverlaySlot->SetVerticalAlignment(nullptr != CanvasPanel ? EVerticalAlignment::VAlign_Fill : EVerticalAlignment::VAlign_Center);
		}
	}
}
void UUIGuideRegistrar::NativeDestruct()
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
					SubSystem->UnregistGuideWidget(Tag);
				}
			}
		}

	}

	Super::NativeDestruct();
}


void UUIGuideRegistrar::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	UOverlaySlot* OverlaySlot = Cast<UOverlaySlot>(NamedSlot->Slot);
	if (OverlaySlot)
	{
		UWidget* NamedSlotWidget = NamedSlot->GetChildAt(0);
		UCanvasPanel* CanvasPanel = Cast<UCanvasPanel>(NamedSlotWidget);

		OverlaySlot->SetHorizontalAlignment(nullptr != CanvasPanel ? EHorizontalAlignment::HAlign_Fill : EHorizontalAlignment::HAlign_Center);
		OverlaySlot->SetVerticalAlignment(nullptr != CanvasPanel ? EVerticalAlignment::VAlign_Fill : EVerticalAlignment::VAlign_Center);
	}


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
void UUIGuideRegistrar::ReleaseSlateResources(bool bReleaseChildren)
{
#if WITH_EDITOR

	PreviewGuideLayer.Reset();
	WidgetPool.ReleaseAll();
#endif


	Super::ReleaseSlateResources(bReleaseChildren);
}
const FText UUIGuideRegistrar::GetPaletteCategory()
{
	return LOCTEXT("UIGuideMask", "UIGuideMask");
}
TSharedRef<SWidget> UUIGuideRegistrar::RebuildWidget()
{
#if WITH_EDITOR

	WidgetPool.SetWorld(GetWorld());

#endif

	return Super::RebuildWidget();
}
