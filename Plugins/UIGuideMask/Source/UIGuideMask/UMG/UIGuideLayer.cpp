// Fill out your copyright notice in the Description page of Project Settings.


#include "UIGuideLayer.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/SizeBox.h"

void UUIGuideLayer::Set(const FGeometry& InBoxGemetry, const UWidget* InWidget)
{
	if (nullptr == LayerPanel || nullptr == InWidget) return;

	TSharedPtr<SWidget> S_Target = InWidget->GetCachedWidget();
	TSharedPtr<SWidget> S_Canvas = LayerPanel->GetCachedWidget();

	if (S_Target && S_Canvas)
	{
		FGeometry CanvasGeometry = S_Canvas->GetTickSpaceGeometry();
		FGeometry TargetGeometry = S_Target->GetTickSpaceGeometry();

		// 1. Target의 TopLeft, BottomRight를 Canvas Panel의 로컬 좌표로 변환
		FVector2D BoxLocalTopLeft = InBoxGemetry.AbsoluteToLocal(TargetGeometry.LocalToAbsolute(TargetGeometry.GetLocalPositionAtCoordinates(FVector2D(0,0))));
		FVector2D BoxLocalBottomRight = InBoxGemetry.AbsoluteToLocal(TargetGeometry.LocalToAbsolute(TargetGeometry.GetLocalSize()));


		// 2. Target의 TopLeft, BottomRight를 Canvas Panel의 로컬 좌표로 변환
		FVector2D TargetLocalTopLeft = CanvasGeometry.AbsoluteToLocal(BoxLocalTopLeft);
		FVector2D TargetLocalBottomRight = CanvasGeometry.AbsoluteToLocal(BoxLocalBottomRight);

		FVector2D TargetLocalSize = TargetLocalBottomRight - TargetLocalTopLeft;

		if (nullptr != GuideBoxPanel)
		{
			if (UCanvasPanelSlot* PanelSlot = Cast<UCanvasPanelSlot>(GuideBoxPanel->Slot))
			{
				PanelSlot->SetAnchors(FAnchors(0, 0, 0, 0));
				PanelSlot->SetSize(TargetLocalSize);
				PanelSlot->SetPosition(TargetLocalTopLeft);
			}

		}
	}
}
