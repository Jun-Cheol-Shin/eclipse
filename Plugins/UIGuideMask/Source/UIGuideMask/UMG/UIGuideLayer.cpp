// Fill out your copyright notice in the Description page of Project Settings.


#include "UIGuideLayer.h"
#include "UIGuideTooltip.h"

#include "Components/Image.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/SizeBox.h"

void UUIGuideLayer::OnResizedViewport(FViewport* InViewport, uint32 InMessage)
{
	// todo
}

void UUIGuideLayer::NativeConstruct()
{
	Super::NativeConstruct();

	FViewport::ViewportResizedEvent.AddUObject(this, &UUIGuideLayer::OnResizedViewport);
}

void UUIGuideLayer::NativeDestruct()
{
	FViewport::ViewportResizedEvent.RemoveAll(this);

	Super::NativeDestruct();
}


void UUIGuideLayer::Set(const FGeometry& InGeometry, const UWidget* InWidget, const FGuideMessageParameters& InMessageParam)
{
	if (nullptr == LayerPanel || nullptr == InWidget) return;

	TSharedPtr<SWidget> S_Target = InWidget->GetCachedWidget();

	if (S_Target)
	{
		ForceLayoutPrepass();

		FVector2D TargetLocalSize = UUIGuideFunctionLibrary::GetWidgetLocalSize(InGeometry, InWidget->GetTickSpaceGeometry());
		FVector2D TargetLocalPosition = InGeometry.AbsoluteToLocal(InWidget->GetTickSpaceGeometry().AbsolutePosition);
		FVector2D TargetLocation = InGeometry.GetLocalPositionAtCoordinates(FVector2D(0, 0)) + TargetLocalPosition;


		if (nullptr != GuideBoxPanel)
		{
			if (UCanvasPanelSlot* PanelSlot = Cast<UCanvasPanelSlot>(GuideBoxPanel->Slot))
			{
				PanelSlot->SetAnchors(FAnchors(0, 0, 0, 0));
				PanelSlot->SetSize(TargetLocalSize);
				PanelSlot->SetPosition(TargetLocation);

			}
		}

		if (nullptr != GuideTooltip)
		{
			if (UCanvasPanelSlot* PanelSlot = Cast<UCanvasPanelSlot>(GuideTooltip->Slot))
			{
				PanelSlot->SetAnchors(FAnchors(0, 0, 0, 0));
				PanelSlot->SetPosition(FVector2D(0, 0));
				PanelSlot->SetAutoSize(true);
			}

			GuideTooltip->SetVisibility(true == InMessageParam.IsEmpty() ? ESlateVisibility::Collapsed : ESlateVisibility::HitTestInvisible);
			if (InMessageParam.IsEmpty())
			{
				return;
			}

			GuideTooltip->ForceLayoutPrepass();
			FVector2D ScreenSize = InGeometry.GetLocalPositionAtCoordinates(FVector2D(0.5, 0.5)) * 2.f;
			FVector2D TooltipSize = GuideTooltip->GetDesiredSize();
	
			const FVector2D Position = GuideTooltip->GetTooltipPosition(
				InMessageParam.TooltipPositionType,
				ScreenSize,
				TargetLocation,
				TargetLocalSize,
				TooltipSize);

			GuideTooltip->SetRenderTranslation(Position + InMessageParam.Offset);

			if (false == InMessageParam.bUseTitle)
			{
				GuideTooltip->Set(
					FTextFormat::FromString(InMessageParam.Format), 
					FFormatOrderedArguments(InMessageParam.FormatArguments));
			}

			else
			{
				GuideTooltip->Set(
					FTextFormat::FromString(InMessageParam.TitleFormat), 
					FFormatOrderedArguments(InMessageParam.TitleFormatArguments),
					FTextFormat::FromString(InMessageParam.Format),
					FFormatOrderedArguments(InMessageParam.FormatArguments));
			}
		}

	}
}