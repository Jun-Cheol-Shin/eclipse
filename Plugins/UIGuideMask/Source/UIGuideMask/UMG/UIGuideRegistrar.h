// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "UIGuideRegistrar.generated.h"

class USizeBox;
class UCanvasPanel;
class UUIGuideLayer;

UCLASS(meta = (DisplayName = "UI Guide Registrar", Category = "UI_Guide"))
class UIGUIDEMASK_API UUIGuideRegistrar : public UUserWidget
{
	GENERATED_BODY()

private:
#if WITH_EDITOR
	UFUNCTION()
	TArray<FName> GetTagOptions() const;

	UFUNCTION(BlueprintCosmetic, CallInEditor, meta = (Category = "UI Guide Mask Preview"))
	void ShowPreviewDebug();

	bool GetTaggedWidget(OUT UWidget** OutWidget);

#endif

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void SynchronizeProperties() override;
	virtual const FText GetPaletteCategory() override;


#if WITH_EDITORONLY_DATA
private:
	UPROPERTY(EditInstanceOnly, meta = (Category = "UI Guide Mask Preview", GetOptions = "GetTagOptions", AllowPrivateAccess = "true"))
	FName TagName;

	UPROPERTY(EditDefaultsOnly, meta = (Category = "UI Guide Mask Preview", GetOptions = "GetTagOptions", AllowPrivateAccess = "true"))
	TSoftClassPtr<UUIGuideLayer> PreviewGuideLayer; 


	TArray<FGameplayTag> RegistedTag{};


	UUIGuideLayer* LoadedLayer = nullptr;
#endif

	
};
