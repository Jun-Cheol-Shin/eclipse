// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../RedDotStatic.h"
#include "RedDot.generated.h"

class FRedDotNode;
class UTextBlock;

UCLASS()
class REDNOTIFIICATIONDOT_API URedDot : public UUserWidget
{
	GENERATED_BODY()

private:
	void OnChangedVisible(bool bIsVisible);

public:
	UFUNCTION(BlueprintCallable)
	void On();

	UFUNCTION(BlueprintCallable)
	void Off();

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void SynchronizeProperties() override;

private:
	void SetCountText();
	
private:
	UPROPERTY(EditInstanceOnly, meta = (AllowPrivateAccess = "true", Category = "RedDot Tag Setting"))
	FGameplayTag ParentTag;

	UPROPERTY(EditInstanceOnly, meta = (AllowPrivateAccess = "true", Category = "RedDot Tag Setting", InlineEditConditionToggle))
	bool bEnableIncludeChild = false;

	UPROPERTY(EditInstanceOnly, meta = (AllowPrivateAccess = "true", Category = "RedDot Tag Setting", EditCondition = "bEnableIncludeChild"))
	FGameplayTag MyTag;


	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true", Category = "Reddot UI Setting", EditCondition = "bEnableIncludeChild"))
	bool bShowCount = false;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true", Category = "Reddot UI Setting", EditCondition = "bShowCount"))
	bool bUseGrouping = false;
	
#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true", Category = "Reddot UI Setting", EditCondition = "bShowCount"))
	int32 PreviewCount = 0;
#endif


private:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* CountText = nullptr;

	TWeakPtr<FRedDotNode> RedDotNode = nullptr;


public:
	static TSharedPtr<FRedDotNode> CreateNode(const FGameplayTag& InMyTag);
	static void RemoveNode(const FGameplayTag& InMyTag);
	void ClearGraph();

private:
	static TMap<FGameplayTag, TSharedPtr<FRedDotNode>> RedDotGraph;
	
};
