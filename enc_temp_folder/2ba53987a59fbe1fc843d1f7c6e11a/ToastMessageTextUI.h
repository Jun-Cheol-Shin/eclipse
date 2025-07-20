// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonNotifyMessageUI.h"
#include "ToastMessageTextUI.generated.h"

/**
 * 
 */
UCLASS()
class NOTIFYUI_API UToastMessageTextUI : public UCommonNotifyMessageUI
{
	GENERATED_BODY()

public:
	DECLARE_DELEGATE_OneParam(FOnCompleteAnimEndSignature, const FGameplayTag&)
	FOnCompleteAnimEndSignature OnCompleteAnimEndDelegate;
	

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;


private:
	UFUNCTION()
	void OnCompleteFadeInAnimation();
	
	// Widget Variable
private:
	UPROPERTY(meta = (BindWidgetAnim, AllowPrivateAccess = "true"), Transient)
	UWidgetAnimation* Up;

	UPROPERTY(meta = (BindWidgetAnim, AllowPrivateAccess = "true"), Transient)
	UWidgetAnimation* Down;

	UPROPERTY(meta = (BindWidgetAnim, AllowPrivateAccess = "true"), Transient)
	UWidgetAnimation* Left;

	UPROPERTY(meta = (BindWidgetAnim, AllowPrivateAccess = "true"), Transient)
	UWidgetAnimation* Right;
	
	UPROPERTY(meta = (BindWidgetAnim, AllowPrivateAccess = "true"), Transient)
	UWidgetAnimation* FadeOut;
};
