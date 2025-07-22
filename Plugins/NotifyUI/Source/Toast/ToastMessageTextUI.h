// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonNotifyMessageUI.h"
#include "ToastMessageTextUI.generated.h"

class USizeBox;
enum class EToastStackType : uint8;

UENUM(BlueprintType)
enum class EAnimationType : uint8
{
	FadeIn_Veritcal,
	FadeIn_Horizontal,
};

UCLASS()
class NOTIFYUI_API UToastMessageTextUI : public UCommonNotifyMessageUI
{
	GENERATED_BODY()

public:
	DECLARE_DELEGATE_OneParam(FOnCompleteAnimEndSignature, UToastMessageTextUI*)
	FOnCompleteAnimEndSignature OnCompleteAnimEndDelegate;

	void Start(const FText& InText, bool bReversed = false);
	void End();
	
	void SetDirection(bool bReversed = false);

private:
	void PlayToastAnimation();

private:
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	EAnimationType AnimationType;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void RemoveFromParent() override;


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

	UPROPERTY(meta = (BindWidget, AllowPrivateAccess = "true"))
	USizeBox* SizeBox;

};
