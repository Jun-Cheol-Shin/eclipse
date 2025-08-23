// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidgetPool.h"
#include "UIGuideLayer.h"
#include "UIGuideRegistrar.generated.h"


class UOverlay;
class UBorder;
class UUIGuideLayer;

UCLASS(meta = (DisplayName = "UI Guide Registrar", Category = "UI_Guide", ToolTip = "If Not Use CanvasPanel, Set Fill Screen Mode!"))
class UIGUIDEMASK_API UUIGuideRegistrar : public UUserWidget
{
	GENERATED_BODY()

public:
	FGuideParameter GetOption(const FGameplayTag& InTag) const;

private:
#if WITH_EDITOR
	UFUNCTION()
	TArray<FName> GetTagOptions() const;

	UFUNCTION(BlueprintCosmetic, CallInEditor, meta = (Category = "UI Guide Mask Preview", DisplayName = "Show Debug"))
	void ShowPreviewDebug();

	UFUNCTION(BlueprintCosmetic, CallInEditor, meta = (Category = "UI Guide Mask Preview", DisplayName = "Hide Debug"))
	void HidePreviewDebug();

	void CreatePreviewLayer();

	// Helper
	bool GetPreviewWidget(OUT UWidget** OutWidget);
	FGameplayTag GetTag(const FName& InTagName);

#endif

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditDefaultsOnly, meta = (Category = "UI Guide Mask Preview", AllowPrivateAccess = "true"))
	TSoftClassPtr<UUIGuideLayer> PreviewGuideLayer;

	UPROPERTY(EditInstanceOnly, meta = (Category = "UI Guide Mask Preview", GetOptions = "GetTagOptions", AllowPrivateAccess = "true"))
	FName PreviewWidgetTag;

	TMap<FGameplayTag, UWidget* /* OuterWidget */> RegistedTagList;
	FUserWidgetPool WidgetPool{ };


#endif

private:
	UPROPERTY(EditInstanceOnly, meta = (Category = "UI Guide Mask Preview", AllowPrivateAccess = "true"))
	TMap<FGameplayTag, FGuideParameter> GuideOption;



protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void SynchronizeProperties() override;
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	virtual const FText GetPaletteCategory() override;

private:
	UPROPERTY(meta = (BindWidget, AllowPrivateAccess = "true"))
	UNamedSlot* NamedSlot;

	UPROPERTY(meta = (BindWidget, AllowPrivateAccess = "true"))
	UOverlay* PreviewOverlay;

};
