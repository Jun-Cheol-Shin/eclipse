// Fill out your copyright notice in the Description page of Project Settings.


#include "UIGuidePanel.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/SizeBox.h"


#include "Components/PanelWidget.h"
#include "../UIGuideMaskable.h"
#include "../Subsystem/UIGuideMaskSubsystem.h"

#define LOCTEXT_NAMESPACE "UMG"

void UUIGuidePanel::NativeConstruct()
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

	if (nullptr != SizeBox)
	{
		SizeBox->SetVisibility(bShowDebug ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
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


	}
}
const FText UUIGuidePanel::GetPaletteCategory()
{
	return LOCTEXT("UIGuideMask", "UIGuideMask");
}
#endif