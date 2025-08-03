// Fill out your copyright notice in the Description page of Project Settings.


#include "TooltipBehavior.h"

#include "../UIGuideMask/Subsystem/UIGuideMaskSubsystem.h"


// Add default functionality here for any ITooltipBehavior functions that are not pure virtual.

void ITooltipBehavior::GetPosition(OUT FVector2D& OutLocation, ETooltipPosition InPosition, const FVector2D& InScreen, const FVector2D& InPivotPosition, const FVector2D& InPivotSize, const FVector2D& InTooltipSize) const
{
	FVector2D DesiredPosition;

	// 타겟의 중심점 (0.5, 0.5)
	const FVector2D& AnchorCenter = InPivotPosition + InPivotSize * 0.5f;

	switch (InPosition)
	{
		case ETooltipPosition::Top:
		DesiredPosition = FVector2D(
			AnchorCenter.X - InTooltipSize.X * 0.5f,
			InPivotPosition.Y - InTooltipSize.Y
		);
		break;

	case ETooltipPosition::TopLeft:
		DesiredPosition = FVector2D(
			InPivotPosition.X - InTooltipSize.X,
			InPivotPosition.Y - InTooltipSize.Y
		);
		break;

	case ETooltipPosition::BottomLeft:
		DesiredPosition = FVector2D(
			InPivotPosition.X - InTooltipSize.X,
			InPivotPosition.Y + InPivotSize.Y
		);
		break;

	case ETooltipPosition::BottomRight:
		DesiredPosition = FVector2D(
			InPivotPosition.X + InPivotSize.X,
			InPivotPosition.Y + InPivotSize.Y
		);
		break;

	case ETooltipPosition::TopRight:
		DesiredPosition = FVector2D(
			InPivotPosition.X + InPivotSize.X,
			InPivotPosition.Y - InTooltipSize.Y
		);
		break;


	case ETooltipPosition::Bottom:
		DesiredPosition = FVector2D(
			AnchorCenter.X - InTooltipSize.X * 0.5f,
			InPivotPosition.Y + InPivotSize.Y
		);
		break;



	case ETooltipPosition::Left:
		DesiredPosition = FVector2D(
			InPivotPosition.X - InTooltipSize.X,
			AnchorCenter.Y - InTooltipSize.Y * 0.5f
		);
		break;

	case ETooltipPosition::Right:
		DesiredPosition = FVector2D(
			InPivotPosition.X + InPivotSize.X,
			AnchorCenter.Y - InTooltipSize.Y * 0.5f
		);
		break;

	case ETooltipPosition::Center:
	default:
		DesiredPosition = AnchorCenter - InTooltipSize * 0.5f;
		break;
	}

	// 화면 밖으로 나가지 않도록 클램프
	DesiredPosition.X = FMath::Clamp(DesiredPosition.X, 0.f, InScreen.X - InTooltipSize.X);
	DesiredPosition.Y = FMath::Clamp(DesiredPosition.Y, 0.f, InScreen.Y - InTooltipSize.Y);

	OutLocation = DesiredPosition;

}


FVector2D UUIGuideFunctionLibrary::GetWidgetLocalSize(const FGeometry& InScreen, const FGeometry& InTarget)
{
	FVector2D TargetLocalBottomRight = InScreen.AbsoluteToLocal(InTarget.LocalToAbsolute(InTarget.GetLocalSize()));
	FVector2D TargetLocalTopLeft = InScreen.AbsoluteToLocal(InTarget.GetAbsolutePosition());

	return TargetLocalBottomRight - TargetLocalTopLeft;
}


void UUIGuideFunctionLibrary::ShowGuideWidget(const UGameInstance* InInstance, FGameplayTag InTag)
{
	UUIGuideMaskSubsystem* GuideSubSystem = InInstance->GetSubsystem<UUIGuideMaskSubsystem>();
	if (ensure(GuideSubSystem))
	{
		GuideSubSystem->ShowGuide(InInstance->GetFirstLocalPlayerController(), InTag);
	}
}

