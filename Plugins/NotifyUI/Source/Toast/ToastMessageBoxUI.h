// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "GameplayTagContainer.h"
#include "ToastMessageTextUI.h"
#include "ToastMessageBoxUI.generated.h"


class UDynamicEntryBox;

UENUM(BlueprintType)
enum class EToastStackType : uint8
{
	Up,
	Down,
};

UCLASS()
class NOTIFYUI_API UToastMessageBoxUI : public UCommonUserWidget
{
	GENERATED_BODY()

private:
	UPROPERTY(EditInstanceOnly, meta = (AllowPrivateAccess = "true", ClampMin = "1", ClampMax = "5"))
	float ToastDuration = 3.f;

	UPROPERTY(EditInstanceOnly, meta = (AllowPrivateAccess = "true", ClampMin = "1", ClampMax = "10"))
	int ToastBoxMaximumCount = 10;

	UPROPERTY(EditInstanceOnly, meta = (AllowPrivateAccess = "true"))
	EToastStackType ToastStackType;

	UPROPERTY(EditInstanceOnly, meta = (AllowPrivateAccess = "true"))
	EAnimationType ToastAnimationType = EAnimationType::Up;

	UPROPERTY(EditInstanceOnly, meta = (AllowPrivateAccess = "true", ClampMin = "1", ClampMax = "10"))
	int PreviewEntryCount = 0;

private:
	UPROPERTY(meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
	UDynamicEntryBox* ToastBox;

protected:
	virtual void SynchronizeProperties() override;
	virtual void NativeConstruct() override;

public:
	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	void RegistToast(FGameplayTag InTag, const FText& InText);

private:
	void OnEndedFadeOutAnimation();

};
