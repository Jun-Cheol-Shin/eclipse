// Fill out your copyright notice in the Description page of Project Settings.


#include "UIGuideRegistrar.h"
#include "UIGuideLayer.h"

#include "Components/ListView.h"
#include "Components/TextBlock.h"
#include "Components/OverlaySlot.h"
#include "Components/Overlay.h"
#include "Components/NamedSlot.h"
#include "Components/PanelWidget.h"

#include "../Interface/UIGuideMaskable.h"
#include "../Subsystem/UIGuideMaskSubsystem.h"
#include "../UIGuideMaskFunctionLibrary.h"

#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"


#define LOCTEXT_NAMESPACE "UMG"


FGuideParameter UUIGuideRegistrar::GetOption(const FGameplayTag& InTag) const
{
	if (GuideOption.Contains(InTag))
	{
		return GuideOption[InTag];
	}

	return FGuideParameter();
}


#if WITH_EDITOR

TArray<FName> UUIGuideRegistrar::GetTagOptions() const
{
	TArray<FGameplayTag> TagList;
	RegistedTagList.GenerateKeyArray(OUT TagList);

	TArray<FName> TagNameArray;

	Algo::Transform(TagList, TagNameArray, [](const FGameplayTag& InTag) -> FName
		{
			return InTag.GetTagName();
		});

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
	if (nullptr != PreviewOverlay)
	{
		for (int i = PreviewOverlay->GetChildrenCount() - 1; i >= 1; --i)
		{
			UWidget* Child = PreviewOverlay->GetChildAt(i);
			if (Child) Child->RemoveFromParent();
		}
	}


	WidgetPool.ReleaseAll();
}

void UUIGuideRegistrar::CreatePreviewLayer()
{
	UWidget* PreviewWidget = nullptr;
	if (false == GetPreviewWidget(&PreviewWidget))
	{
		return;
	}


	UUIGuideLayer* PreviewGuideLayerWidget = WidgetPool.GetOrCreateInstance<UUIGuideLayer>(PreviewGuideLayer.Get());
	if (PreviewGuideLayerWidget)
	{
		FGameplayTag SelectedTag = GetTag(PreviewWidgetTag);

		FGuideParameter Parameter;
		if (GuideOption.Contains(SelectedTag))
		{
			Parameter = GuideOption[SelectedTag];
		}

		else
		{
			UUserWidget* UserWidget = PreviewWidget->GetTypedOuter<UUserWidget>();
			if (UserWidget && UserWidget->WidgetTree)
			{
				TArray<UWidget*> Children;
				UserWidget->WidgetTree->GetAllWidgets(OUT Children);

				for (auto& Child : Children)
				{
					if (UUIGuideRegistrar* Registrar = Cast<UUIGuideRegistrar>(Child))
					{
						Parameter = Registrar->GetOption(SelectedTag);
						break;
					}
				}
			}
		}

		if (nullptr != PreviewOverlay)
		{
			if (UOverlaySlot* OverlaySlot = Cast<UOverlaySlot>(PreviewOverlay->AddChildToOverlay(PreviewGuideLayerWidget)))
			{
				OverlaySlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
				OverlaySlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
			}
		}

	

		PreviewGuideLayerWidget->Set(NamedSlot->GetCachedGeometry(), PreviewWidget, Parameter.LayerParameter, Parameter.MessageParameter, Parameter.bUseAction);
	}
}

bool UUIGuideRegistrar::GetPreviewWidget(OUT UWidget** OutWidget)
{
	FGameplayTag SelectedTag = GetTag(PreviewWidgetTag);
	if (false == RegistedTagList.Contains(SelectedTag)) return false;

	UWidget* OuterWidget = RegistedTagList[SelectedTag];
	UWidget* FoundWidget = UUIGuideMaskFunctionLibrary::GetTagWidget(OuterWidget, SelectedTag);

	while (nullptr != FoundWidget)
	{
		if (FoundWidget->GetClass()->ImplementsInterface(UUIGuideMaskable::StaticClass()))
		{
			FoundWidget = UUIGuideMaskFunctionLibrary::GetTagWidget(Cast<UUserWidget>(FoundWidget), SelectedTag);
		}

		else
		{
			break;
		}
	}

	if (FoundWidget)
	{
		*OutWidget = FoundWidget;
		return true;
	}

	return false;
}
FGameplayTag UUIGuideRegistrar::GetTag(const FName& InTagName)
{
	TArray<FGameplayTag> TagList;
	RegistedTagList.GenerateKeyArray(OUT TagList);

	for (auto& Tag : TagList)
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
					NewData.OuterWidget = OwnerUserWidget;

					FGuideParameter NewParameter;
					if (true == GuideOption.Contains(Tag))
					{
						NewParameter = GuideOption[Tag];
					}

					NewData.GuideParameters = NewParameter;
					SubSystem->RegistGuideWidget(NewData);
				}
			}
		}

		/*UBorderSlot* BorderSlot = Cast<UBorderSlot>(NamedSlot->Slot);
		if (BorderSlot)
		{
			UWidget* NamedSlotWidget = NamedSlot->GetChildAt(0);
			UCanvasPanel* CanvasPanel = Cast<UCanvasPanel>(NamedSlotWidget);

			BorderSlot->SetHorizontalAlignment(nullptr != CanvasPanel ? EHorizontalAlignment::HAlign_Fill : EHorizontalAlignment::HAlign_Center);
			BorderSlot->SetVerticalAlignment(nullptr != CanvasPanel ? EVerticalAlignment::VAlign_Fill : EVerticalAlignment::VAlign_Center);
		}
		*/

		PreviewOverlay = nullptr;
		RegistedTagList.Reset();
		WidgetPool.ReleaseAll();

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

	PreviewOverlay = nullptr;

	Super::NativeDestruct();
}


void UUIGuideRegistrar::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	if (NamedSlot && NamedSlot->GetChildrenCount() <= 0)
	{
		FString ParentWidgetName = GetNameSafe(this);

		if (UUserWidget* OuterWidget = GetTypedOuter<UUserWidget>())
		{
			ParentWidgetName = GetNameSafe(OuterWidget);
		}


		UTextBlock* TextBlock = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
		TextBlock->SetText(FText::FromString(FString::Printf(TEXT("Change the screen mode of %s to fill screen mode."), *ParentWidgetName)));
		TextBlock->SetColorAndOpacity(FLinearColor::Gray);

		if (PreviewOverlay)
		{
			if (UOverlaySlot* OverlaySlot = PreviewOverlay->AddChildToOverlay(TextBlock))
			{
				OverlaySlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Left);
				OverlaySlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Top);
			}
		}
	}


	if (NamedSlot)
	{
		UOverlaySlot* OverlaySlot = Cast<UOverlaySlot>(NamedSlot->Slot);
		if (OverlaySlot)
		{
			UWidget* NamedSlotWidget = NamedSlot->GetChildAt(0);
			UCanvasPanel* CanvasPanel = Cast<UCanvasPanel>(NamedSlotWidget);

			OverlaySlot->SetHorizontalAlignment(nullptr != CanvasPanel ? EHorizontalAlignment::HAlign_Fill : EHorizontalAlignment::HAlign_Center);
			OverlaySlot->SetVerticalAlignment(nullptr != CanvasPanel ? EVerticalAlignment::VAlign_Fill : EVerticalAlignment::VAlign_Center);
		}
	}



#if WITH_EDITOR

	RegistedTagList.Reset();

	if (UUserWidget* OwnerUserWidget = GetTypedOuter<UUserWidget>())
	{
		if (OwnerUserWidget->GetClass()->ImplementsInterface(UUIGuideMaskable::StaticClass()))
		{
			TMap<FGameplayTag, UWidget*> Map = IUIGuideMaskable::Execute_OnGetMaskableWidget(OwnerUserWidget);

			for (auto& [Tag, Widget] : Map)
			{
				RegistedTagList.Emplace(Tag, OwnerUserWidget);
			}
		}
	}

	if (NamedSlot)
	{
		for (auto& Child : NamedSlot->GetAllChildren())
		{
			UUIGuideMaskFunctionLibrary::ForEachWidgetRecursive(Child, [&](UWidget* InRoot)
				{
					check(InRoot);
					if (InRoot->GetClass()->ImplementsInterface(UUIGuideMaskable::StaticClass()))
					{
						TMap<FGameplayTag, UWidget*> Map = IUIGuideMaskable::Execute_OnGetMaskableWidget(InRoot);

						for (auto& [Tag, Widget] : Map)
						{
							RegistedTagList.Add(Tag, InRoot);
						}

						UE_LOG(LogTemp, Log, TEXT("Found: %s (%s)"), *GetNameSafe(InRoot), *InRoot->GetClass()->GetName());

					}
				});
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
