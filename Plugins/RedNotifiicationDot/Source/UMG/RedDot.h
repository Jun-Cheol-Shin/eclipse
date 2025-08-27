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

	TSharedPtr<FRedDotNode> RedDotNode = nullptr;


public:
	static TSharedPtr<FRedDotNode> CreateNode(const FGameplayTag& InParentTag, const FGameplayTag& InMyTag);
	static void RemoveNode(const FGameplayTag& InMyTag);
	void ClearGraph();

private:
	static TMap<FGameplayTag, TSharedPtr<FRedDotNode>> RedDotGraph;
	
	// TODO : 혹시 모를 부모가 로드가 안됐을 때 대기해야 할 레드닷 위젯 WeakObjectPtr, 부모가 로드되면, 부모를 등록해줘야 함.
	// TArray?
};
