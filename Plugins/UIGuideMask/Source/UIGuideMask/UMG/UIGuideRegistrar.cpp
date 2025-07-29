// Fill out your copyright notice in the Description page of Project Settings.


#include "UIGuideRegistrar.h"
#include "UIGuideLayer.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/SizeBox.h"

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
	if (!HasAnyFlags(RF_ClassDefaultObject))
	{
		// 실제 BP 인스턴스(Placed Actor 등)에서만 실행
		// CDO(BP Asset 에디터)는 무시

		if (LoadedLayer)
		{
			/*LoadedLayer->TakeDerivedWidget([](UUserWidget* Widget, TSharedRef<SWidget> Content)
				{
					return SNew(SObjectWidget, Widget)[Content];
				});*/
		}


		else if(PreviewGuideLayer.IsValid())
		{
			FStreamableManager& StreamableManager = UAssetManager::Get().GetStreamableManager();
			TSharedPtr<FStreamableHandle> StreamingHandle = StreamableManager.RequestAsyncLoad(PreviewGuideLayer.ToSoftObjectPath(), FStreamableDelegate::CreateWeakLambda(this,
				[this]()
				{
					LoadedLayer = CreateWidget<UUIGuideLayer>(this, PreviewGuideLayer.Get());

				}));
		}

	}

	/*
		if (nullptr == CanvasPanel || nullptr == FindWidget) return;

		TSharedPtr<SWidget> S_Target = FindWidget->GetCachedWidget();
		TSharedPtr<SWidget> S_Canvas = CanvasPanel->GetCachedWidget();

		if (S_Target && S_Canvas)
		{
			FGeometry CanvasGeometry = S_Canvas->GetTickSpaceGeometry();
			FGeometry TargetGeometry = S_Target->GetTickSpaceGeometry();

			// 2. Target의 TopLeft, BottomRight를 Canvas Panel의 로컬 좌표로 변환
			FVector2D TargetLocalTopLeft = CanvasGeometry.AbsoluteToLocal(TargetGeometry.LocalToAbsolute(FVector2D::ZeroVector));
			FVector2D TargetLocalBottomRight = CanvasGeometry.AbsoluteToLocal(TargetGeometry.LocalToAbsolute(TargetGeometry.GetLocalSize()));

			FVector2D TargetLocalSize = TargetLocalBottomRight - TargetLocalTopLeft;

			if (nullptr != SizeBox)
			{
				if (UCanvasPanelSlot* PanelSlot = Cast<UCanvasPanelSlot>(SizeBox->Slot))
				{
					PanelSlot->SetAnchors(FAnchors(0, 0, 0, 0));
					PanelSlot->SetSize(TargetLocalSize);
					PanelSlot->SetPosition(TargetLocalTopLeft);
				}

			}
		}
		*/



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
const FText UUIGuideRegistrar::GetPaletteCategory()
{
	return LOCTEXT("UIGuideMask", "UIGuideMask");
}