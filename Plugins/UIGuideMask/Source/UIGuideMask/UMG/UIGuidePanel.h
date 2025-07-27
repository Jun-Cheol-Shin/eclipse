// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "UIGuidePanel.generated.h"

class USizeBox;
class UCanvasPanel;

UCLASS(meta = (DisplayName = "UI Guide Panel", Category = "UI_Guide"))
class UIGUIDEMASK_API UUIGuidePanel : public UUserWidget
{
	GENERATED_BODY()



protected:
	//virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void NativeConstruct() override;


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


#if WITH_EDITORONLY_DATA
private:
	UPROPERTY(EditInstanceOnly, meta = (Category = "UI Guide Mask Preview", GetOptions = "GetTagOptions", AllowPrivateAccess = "true"))
	FName TagName;

	UPROPERTY(EditInstanceOnly, meta = (Category = "UI Guide Mask Preview", AllowPrivateAccess = "true"))
	bool bShowDebug = false;

	TArray<FGameplayTag> RegistedTag{};
#endif


private:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	UCanvasPanel* CanvasPanel;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	USizeBox* SizeBox;
	
};
