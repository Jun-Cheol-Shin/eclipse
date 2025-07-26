// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/CanvasPanel.h"
#include "GameplayTagContainer.h"
#include "UIGuidePanel.generated.h"

/**
 * 
 */
UCLASS(meta = (DisplayName = "UI Guide Panel", Category = "UI_Guide"))
class UIGUIDEMASK_API UUIGuidePanel : public UCanvasPanel
{
	GENERATED_BODY()

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	virtual void SynchronizeProperties() override;
	

#if WITH_EDITOR
private:
	UFUNCTION()
	TArray<FName> GetTagOptions() const;

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual const FText GetPaletteCategory() override;

#endif

protected:
	// UPanelWidget
	//virtual UClass* GetSlotClass() const override;
	//virtual void OnSlotAdded(UPanelSlot* InSlot) override;
	//virtual void OnSlotRemoved(UPanelSlot* InSlot) override;
	// End UPanelWidget


#if WITH_EDITORONLY_DATA
private:
	UPROPERTY(EditInstanceOnly, meta = (Category = "UI Guide Mask Preview", GetOptions = "GetTagOptions", AllowPrivateAccess = "true"))
	FName TagName;

	UPROPERTY(EditInstanceOnly, meta = (Category = "UI Guide Mask Preview", AllowPrivateAccess = "true"))
	bool bShowDebug = false;

	TArray<FGameplayTag> RegistedTag{};
#endif
	
};
