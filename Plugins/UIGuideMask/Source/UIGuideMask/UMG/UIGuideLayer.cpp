// Fill out your copyright notice in the Description page of Project Settings.


#include "UIGuideLayer.h"
#include "UIGuideTooltip.h"
#include "UIGuideMaskBox.h"
#include "UIGuideRegistrar.h"

#include "Components/Image.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/SizeBox.h"

#include "Blueprint/WidgetLayoutLibrary.h"

void UUIGuideLayer::OnResizedViewport(FViewport* InViewport, uint32 InMessage)
{
	// todo
}

void UUIGuideLayer::Set(const FGeometry& InGeometry, UWidget* InWidget, const FGuideParameter& InParam)
{
	if (nullptr == LayerPanel || nullptr == InWidget) return;

	InWidget->ForceLayoutPrepass();
	ForceLayoutPrepass();

	FGuideMessageParameters MessageParam = InParam.MessageParameter;
	FGuideLayerParameters LayerParam = InParam.LayerParameter;
	FGuideBoxActionParameters ActionParam = InParam.AcitonParameter;
	
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

	if (ensure(MaterialInstance))
	{
		MaterialInstance->SetScalarParameterValue(TEXT("Opacity"), Opacity);
		MaterialInstance->SetScalarParameterValue(TEXT("Shape"), true == LayerParam.bUseHighlightCircleShape ? 1.f : 0.f);
	}

	FVector2D ScreenSize = InGeometry.GetLocalPositionAtCoordinates(FVector2D(0.5, 0.5)) * 2.f;
	SetMaterialTransform(ScreenSize, TargetLocation, TargetLocalSize, LayerParam.Padding);

	if (nullptr == GuideBoxPanel) return;
	if (UCanvasPanelSlot* BoxSlot = Cast<UCanvasPanelSlot>(GuideBoxPanel->Slot))
	{
		BoxSlot->SetSize(TargetLocalSize + LayerParam.Padding);
		BoxSlot->SetPosition(TargetLocation - LayerParam.Padding * 0.5f);
	}

	if (nullptr != GuideTooltip)
	{
		if (UCanvasPanelSlot* PanelSlot = Cast<UCanvasPanelSlot>(GuideTooltip->Slot))
		{
			PanelSlot->SetAnchors(FAnchors(0, 0, 0, 0));
			PanelSlot->SetPosition(FVector2D(0, 0));
			PanelSlot->SetAutoSize(true);
		}

		GuideTooltip->SetVisibility(true == MessageParam.IsEmpty() ? ESlateVisibility::Collapsed : ESlateVisibility::HitTestInvisible);
		if (false == MessageParam.IsEmpty())
		{
			GuideTooltip->ForceLayoutPrepass();

			FVector2D TooltipSize = GuideTooltip->GetTickSpaceGeometry().GetLocalSize();

			const FVector2D Position = GuideTooltip->GetTooltipPosition(
				MessageParam.TooltipPositionType,
				ScreenSize,
				TargetLocation,
				TargetLocalSize,
				TooltipSize);

			GuideTooltip->SetRenderTranslation(Position + MessageParam.Offset);

			if (false == MessageParam.bUseTitle)
			{
				GuideTooltip->Set(
					FTextFormat::FromString(MessageParam.Format),
					FFormatOrderedArguments(MessageParam.FormatArguments));
			}

			else
			{
				GuideTooltip->Set(
					FTextFormat::FromString(MessageParam.TitleFormat),
					FFormatOrderedArguments(MessageParam.TitleFormatArguments),
					FTextFormat::FromString(MessageParam.Format),
					FFormatOrderedArguments(MessageParam.FormatArguments));
			}
		}
	}

	if (nullptr != GuideMaskBox && nullptr != GuideBoxPanel)
	{
		if (InParam.bUseAction)
		{
			GuideBoxPanel->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			GuideMaskBox->SetBox(InWidget, ActionParam);
		}

		else
		{
			GuideBoxPanel->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

}

void UUIGuideLayer::SetMaterialTransform(const FVector2D& InViewportSize, const FVector2D& InPosiiton, const FVector2D& InWidgetSize, const FVector2D& InOffset)
{
	// 실제 픽셀 좌표로 변환
	FVector2D WidgetLeftTop = FVector2D(InPosiiton.X + InWidgetSize.X * 0.5f, InPosiiton.Y + InWidgetSize.Y * 0.5f);
	FVector2D WidgetSize = InWidgetSize;

	if (false == InOffset.IsZero())
	{
		WidgetSize += InOffset;
	}

	float DPIScale = UWidgetLayoutLibrary::GetViewportScale(this);

	FVector2D WidgetCenter_Pixel = WidgetLeftTop * DPIScale;
	FVector2D WidgetSize_Pixel = WidgetSize * 0.5f * DPIScale;

	// UV 변환
	FVector2D CenterUV = WidgetCenter_Pixel / InViewportSize;
	FVector2D SizeUV = WidgetSize_Pixel / InViewportSize;

	// 머티리얼 파라미터로 넘기기
	if (ensure(MaterialInstance))
	{
		MaterialInstance->SetVectorParameterValue("Center", FLinearColor(CenterUV.X, CenterUV.Y, 0, 0));
		MaterialInstance->SetVectorParameterValue("Size", FLinearColor(SizeUV.X, SizeUV.Y, 0, 0));
	}
}

void UUIGuideLayer::NativeConstruct()
{
	Super::NativeConstruct();

	MaterialInstance = BlackScreen->GetDynamicMaterial();
	if (ensure(MaterialInstance))
	{
		FViewport::ViewportResizedEvent.AddUObject(this, &UUIGuideLayer::OnResizedViewport);
	}

	if (nullptr != LayerPanel)
	{
		LayerPanel->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}

	if (nullptr != BlackScreen)
	{
		BlackScreen->SetVisibility(ESlateVisibility::Visible);
	}

	if (nullptr != GuideBoxPanel)
	{
		GuideBoxPanel->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}

	if (nullptr != GuideTooltip)
	{
		GuideTooltip->SetVisibility(ESlateVisibility::HitTestInvisible);
	}

	if (nullptr != GuideMaskBox)
	{
		GuideMaskBox->SetVisibility(ESlateVisibility::Visible);
	}

	SetConsumePointerInput(true);
}

void UUIGuideLayer::NativeDestruct()
{
	FViewport::ViewportResizedEvent.RemoveAll(this);
	MaterialInstance = nullptr;

	Super::NativeDestruct();
}


#if WITH_EDITOR
void UUIGuideLayer::ShowPreviewDebug()
{
	ForceLayoutPrepass();

	MaterialInstance = BlackScreen->GetDynamicMaterial();
	if (ensure(MaterialInstance))
	{
		MaterialInstance->SetScalarParameterValue(TEXT("Opacity"), Opacity);
		MaterialInstance->SetScalarParameterValue(TEXT("Shape"), true == bUseCircle ? 1.f : 0.f);
	}

	SetMaterialTransform(GetTickSpaceGeometry().GetLocalSize(), ScreenPosition, HighlightSize);

	if (nullptr == GuideBoxPanel) return;
	if (UCanvasPanelSlot* BoxSlot = Cast<UCanvasPanelSlot>(GuideBoxPanel->Slot))
	{
		BoxSlot->SetSize(HighlightSize);
		BoxSlot->SetPosition(ScreenPosition);
	}
}
#endif