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
	void OnChangedVisible(bool bIsVisible, int32 InVisibleCount);

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
	UPROPERTY(EditInstanceOnly, meta = (AllowPrivateAccess = "true", Categoryd = "RedDot Tag Setting"))
	FGameplayTag ParentTag;

	UPROPERTY(EditInstanceOnly, meta = (AllowPrivateAccess = "true", Categoryd = "RedDot Tag Setting"))
	FGameplayTag MyTag;


	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true", Category = "Reddot UI Setting"))
	bool bShowCount = false;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true", Category = "Reddot UI Setting", EditCondition = "true == bShowCount"))
	int32 Count = 0;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true", Category = "Reddot UI Setting", EditCondition = "true == bShowCount"))
	bool bUseGrouping = false;
	

private:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* CountText = nullptr;

	TWeakPtr<FRedDotNode> RedDotNode = nullptr;


public:
	static TSharedPtr<FRedDotNode> CreateNode(const FGameplayTag& InParentTag, const FGameplayTag& InMyTag);
	static bool GetNode(const FGameplayTag& InTag, OUT TSharedPtr<FRedDotNode>* OutNode);
	static void RemoveNode(const FGameplayTag& InMyTag);
	void ClearGraph();

private:
	static TMap<FGameplayTag, TSharedPtr<FRedDotNode>> RedDotGraph;
};
