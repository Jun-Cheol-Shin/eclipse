// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameplayTagContainer.h"
#include "TooltipBehavior.generated.h"

UENUM(BlueprintType)
enum class ETooltipPosition : uint8
{
	Top = 0,
	TopLeft,
	TopRight,
	Right,
	BottomRight,
	Bottom,
	BottomLeft,
	Left,
	Center,
};


// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UTooltipBehavior : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class UIGUIDEMASK_API ITooltipBehavior
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
protected:
	void GetPosition(OUT FVector2D& OutLocation, ETooltipPosition InPosition, const FVector2D& InScreen, const FVector2D& InPivotPosition, const FVector2D& InPivotSize, const FVector2D& InTooltipSize) const;
	FVector2D GetWidgetLocalSize(const FGeometry& InScreen, const FGeometry& InTarget);
};
