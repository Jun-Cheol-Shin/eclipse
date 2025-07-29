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
	// 실제 BP 인스턴스(Placed Actor 등)에서만 실행
	// CDO(BP Asset 에디터)는 무시
	WidgetPool.ResetPool();
	WidgetPool.SetWorld(GetWorld());

	if (PreviewGuideLayer.IsValid())
	{
		UUIGuideLayer* NewLayer = WidgetPool.GetOrCreateInstance<UUIGuideLayer>(PreviewGuideLayer.Get());
		if (NewLayer)
		{
			if (UOverlaySlot* OverlaySlot = GuideOverlay->AddChildToOverlay(NewLayer))
			{
				OverlaySlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
				OverlaySlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
			}
			// set

			UWidget* TaggedWidget = nullptr;
			if (true == GetTaggedWidget(&TaggedWidget))
			{

				NewLayer->Set(NamedSlot->GetTickSpaceGeometry(), TaggedWidget);
			}
		}
	}


	else if (PreviewGuideLayer.ToSoftObjectPath().IsValid())
	{
		FStreamableManager& StreamableManager = UAssetManager::Get().GetStreamableManager();
		TSharedPtr<FStreamableHandle> StreamingHandle = StreamableManager.RequestAsyncLoad(PreviewGuideLayer.ToSoftObjectPath(), FStreamableDelegate::CreateWeakLambda(this,
			[this]()
			{
				UUIGuideLayer* NewLayer = WidgetPool.GetOrCreateInstance<UUIGuideLayer>(PreviewGuideLayer.Get());
				if (NewLayer)
				{
					if (UOverlaySlot* OverlaySlot = GuideOverlay->AddChildToOverlay(NewLayer))
					{
						OverlaySlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
						OverlaySlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
					}

					// set

					UWidget* TaggedWidget = nullptr;
					if (true == GetTaggedWidget(&TaggedWidget))
					{
						NewLayer->Set(NamedSlot->GetTickSpaceGeometry(), TaggedWidget);
					}
				}

			}));
	}

}

void UUIGuideRegistrar::HidePreviewDebug()
{
	GuideOverlay->RemoveChildAt(1);
	WidgetPool.ReleaseAll();
}

bool UUIGuideRegistrar::GetTaggedWidget(OUT UWidget** OutWidget)
{
	UUserWidget* OwnerUserWidget = GetTypedOuter<UUserWidget>();
	if (nullptr == OwnerUserWidget || this == OwnerUserWidget) return false;

	FGameplayTag FindTag;

	for (auto& Tag : RegistedTag)
	{
		if (Tag.GetTagName().IsEqual(TagName))
		{
			FindTag = Tag;
			break;
		}
	}

	if (true == OwnerUserWidget->GetClass()->ImplementsInterface(UUIGuideMaskable::StaticClass()))
	{
		TMap<FGameplayTag, UWidget*> Map = IUIGuideMaskable::Execute_OnGetMaskableWidget(OwnerUserWidget);

		if (Map.Contains(FindTag))
		{
			*OutWidget = Map[FindTag];

			return true;
		}
	}

	return false;
}

#endif


TSharedRef<SWidget> UUIGuideRegistrar::RebuildWidget()
{
#if WITH_EDITOR
	WidgetPool.SetWorld(GetWorld());
#endif

	return Super::RebuildWidget();
}

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
					//SubSystem->SetGuideWidget(Tag, Widget);
				}
			}
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
					//SubSystem->SetGuideWidget(Tag, Widget);
				}
			}
		}

	}

	Super::NativeDestruct();
}


void UUIGuideRegistrar::SynchronizeProperties()
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