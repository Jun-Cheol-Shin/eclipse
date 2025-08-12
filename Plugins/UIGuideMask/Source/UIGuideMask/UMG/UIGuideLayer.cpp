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

#include "../Subsystem/UIGuideMaskSubsystem.h"

void UUIGuideLayer::OnPostAction(UWidget* InWidget)
{
	UGameInstance* GameInstance = GetGameInstance();
	if (nullptr == GameInstance) return;

	UUIGuideMaskSubsystem* Subsystem = GameInstance->GetSubsystem<UUIGuideMaskSubsystem>();
	if (ensure(Subsystem)) return;

	Subsystem->OnCompleteAction(InWidget);
}

void UUIGuideLayer::Set(const FGeometry& InGeometry, UWidget* InWidget, const FGuideParameter& InParam)
{
	if (nullptr == LayerPanel || nullptr == InWidget) return;

	ForceLayoutPrepass();

	FGuideMessageParameters MessageParam = InParam.MessageParameter;
	FGuideLayerParameters LayerParam = InParam.LayerParameter;
	FGuideBoxActionParameters ActionParam = InParam.AcitonParameter;

	InWidget->ForceLayoutPrepass();

	// Get target location
	FVector2D TargetLocalPosition = InGeometry.AbsoluteToLocal(InWidget->GetTickSpaceGeometry().AbsolutePosition);
	FVector2D TargetLocation = InGeometry.GetLocalPositionAtCoordinates(FVector2D(0, 0)) + TargetLocalPosition;

	// Get screen size
	FVector2D ScreenSize = InGeometry.GetLocalPositionAtCoordinates(FVector2D(0.5, 0.5)) * 2.f;

	// Get target size
	FVector2D TargetLocalBottomRight = InGeometry.AbsoluteToLocal(InWidget->GetTickSpaceGeometry().LocalToAbsolute(InWidget->GetTickSpaceGeometry().GetLocalSize()));
	FVector2D TargetLocalTopLeft = InGeometry.AbsoluteToLocal(InWidget->GetTickSpaceGeometry().GetAbsolutePosition());
	FVector2D TargetLocalSize = TargetLocalBottomRight - TargetLocalTopLeft;


	SetGuideLayer(LayerParam, ScreenSize, TargetLocation, TargetLocalSize);
	SetGuideTooltip(MessageParam, ScreenSize, TargetLocation, TargetLocalSize);
	SetGuideBox(ActionParam, InParam.bUseAction, InWidget);

	UGameInstance* GameInstance = GetGameInstance();
	if (nullptr == GameInstance) return;

	UUIGuideMaskSubsystem* Subsystem = GameInstance->GetSubsystem<UUIGuideMaskSubsystem>();
	if (ensure(Subsystem)) return;

	Subsystem->OnStartGuide(InWidget);
}


void UUIGuideLayer::SetGuideLayer(const FGuideLayerParameters& InLayerParam, const FVector2D& InScreenSize, const FVector2D& InTargetLoc, const FVector2D& InTargetSize)
{
	if (nullptr != GuideBoxPanel)
	{
		if (UCanvasPanelSlot* PanelSlot = Cast<UCanvasPanelSlot>(GuideBoxPanel->Slot))
		{
			PanelSlot->SetAnchors(FAnchors(0, 0, 0, 0));
			PanelSlot->SetSize(InTargetSize);
			PanelSlot->SetPosition(InTargetLoc);
		}
	}

	if (ensure(MaterialInstance))
	{
		MaterialInstance->SetScalarParameterValue(TEXT("Opacity"), Opacity);
		MaterialInstance->SetScalarParameterValue(TEXT("Shape"), true == InLayerParam.bUseHighlightCircleShape ? 1.f : 0.f);
	}


	SetMaterialTransform(InScreenSize, InTargetLoc, InTargetSize, InLayerParam.Padding);

	if (nullptr == GuideBoxPanel) return;
	if (UCanvasPanelSlot* BoxSlot = Cast<UCanvasPanelSlot>(GuideBoxPanel->Slot))
	{
		BoxSlot->SetSize(InTargetSize + InLayerParam.Padding);
		BoxSlot->SetPosition(InTargetLoc - InLayerParam.Padding * 0.5f);
	}
}
void UUIGuideLayer::SetGuideTooltip(const FGuideMessageParameters& InMessageParam, const FVector2D& InScreenSize, const FVector2D& InTargetLoc, const FVector2D& InTargetSize)
{
	if (nullptr != GuideTooltip)
	{
		if (UCanvasPanelSlot* PanelSlot = Cast<UCanvasPanelSlot>(GuideTooltip->Slot))
		{
			PanelSlot->SetAnchors(FAnchors(0, 0, 0, 0));
			PanelSlot->SetPosition(FVector2D(0, 0));
			PanelSlot->SetAutoSize(true);
		}

		GuideTooltip->SetVisibility(true == InMessageParam.IsEmpty() ? ESlateVisibility::Collapsed : ESlateVisibility::HitTestInvisible);
		if (false == InMessageParam.IsEmpty())
		{
			GuideTooltip->ForceLayoutPrepass();

			FVector2D TooltipSize = GuideTooltip->GetTickSpaceGeometry().GetLocalSize();

			const FVector2D Position = GuideTooltip->GetTooltipPosition(
				InMessageParam.TooltipPositionType,
				InScreenSize,
				InTargetLoc,
				InTargetSize,
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
void UUIGuideLayer::SetGuideBox(const FGuideBoxActionParameters& InActionParam, bool bInUseAction, UWidget* InWidget)
{
	if (nullptr != GuideMaskBox && nullptr != GuideBoxPanel)
	{
		if (bInUseAction)
		{
			GuideBoxPanel->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			GuideMaskBox->SetBox(InWidget, InActionParam);
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
		GuideMaskBox->OnPostAction.BindUObject(this, &UUIGuideLayer::OnPostAction);
	}

	SetConsumePointerInput(true);
}
	
void UUIGuideLayer::NativeDestruct()
{
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