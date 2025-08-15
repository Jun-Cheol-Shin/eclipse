// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "UIGuideTooltip.h"
#include "UIGuideMaskBox.h"
#include "UIGuideLayer.generated.h"


class UUIGuideTooltip;

class UCanvasPanel;
class USizeBox;
class UImage;

class UMaterialInstanceDynamic;
struct FGuideParameter;


USTRUCT(BlueprintType)
struct FGuideLayerParameters
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bUseHighlightCircleShape = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bUseAnimation = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D Padding = FVector2D();

};

USTRUCT(BlueprintType)
struct FGuideParameter
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGuideLayerParameters LayerParameter{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGuideMessageParameters MessageParameter{};

	UPROPERTY(EditAnywhere, meta = (InlineEditConditionToggle))
	bool bUseAction = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "true == bUseAction"))
	FGuideBoxActionParameters AcitonParameter {};
};





UCLASS()
class UIGUIDEMASK_API UUIGuideLayer : public UCommonUserWidget
{
	GENERATED_BODY()

	friend class UUIGuideRegistrar;

private:
	// After Action
	void OnPostAction();

public:
	void Set(const FGeometry& InGeometry, UWidget* InWidget, const FGuideParameter& InParam = FGuideParameter());

private:
	void SetGuideLayer(const FGuideLayerParameters& InLayerParam, const FVector2D& InScreenSize, const FVector2D& InTargetLoc, const FVector2D& InTargetSize);
	void SetGuideTooltip(const FGuideMessageParameters& InMessageParam, const FVector2D& InScreenSize, const FVector2D& InTargetLoc, const FVector2D& InTargetSize);
	void SetGuideBox(const FGuideBoxActionParameters& InActionParam, bool bInUseAction, UWidget* InWidget);

private:
#if WITH_EDITOR
	UFUNCTION(BlueprintCosmetic, CallInEditor, meta = (Category = "Preview", DisplayName = "Show Debug"))
	void ShowPreviewDebug();
#endif

	void OnResizedViewport(FViewport* InViewport, uint32 InMessage);
	void SetMaterialTransform(const FVector2D& InViewportSize, const FVector2D& InPosiiton, const FVector2D& InWidgetSize, const FVector2D& InOffset = FVector2D());

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InEvent) override;
	virtual FReply NativeOnTouchEnded(const FGeometry& InGeometry, const FPointerEvent& InEvent) override;

private:
// BP Variables
	UPROPERTY(EditDefaultsOnly, meta = (Category = "Preview", AllowPrivateAccess = "true"))
	ETooltipPosition TooltipPosition;

	UPROPERTY(EditDefaultsOnly, meta = (Category = "Preview", AllowPrivateAccess = "true", ClampMin = "0", ClampMax = "1"))
	float Opacity = 0.8f;

	UPROPERTY(EditDefaultsOnly, meta = (Category = "Preview", AllowPrivateAccess = "true"))
	bool bUseCircle = false;

	UPROPERTY(EditDefaultsOnly, meta = (Category = "Preview", AllowPrivateAccess = "true"))
	FVector2D ScreenPosition;

	UPROPERTY(EditDefaultsOnly, meta = (Category = "Preview", AllowPrivateAccess = "true"))
	FVector2D HighlightSize;


// Binded Widget Varaible
	UPROPERTY(meta = (BindWidget, AllowPrivateAccess = "true"))
	UCanvasPanel* LayerPanel;

	UPROPERTY(meta = (BindWidget, AllowPrivateAccess = "true"))
	UImage* BlackScreen;

	UPROPERTY(meta = (BindWidget, AllowPrivateAccess = "true"))
	USizeBox* GuideBoxPanel;

	UPROPERTY(meta = (BindWidget, AllowPrivateAccess= "true"))
	UUIGuideTooltip* GuideTooltip;	

	UPROPERTY(meta = (BindWidget, AllowPrivateAccess = "true"))
	UUIGuideMaskBox* GuideMaskBox;


private:
	// Class Variables
	UPROPERTY()
	UMaterialInstanceDynamic* MaterialInstance = nullptr;
};
