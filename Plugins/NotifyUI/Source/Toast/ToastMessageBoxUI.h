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


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFinishMessageSignature, FGameplayTag, InMessageType);

UCLASS()
class NOTIFYUI_API UToastMessageBoxUI : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, meta = (Category = "Toast UI"))
	FOnFinishMessageSignature OnFinishMessageSignature;


private:
	UPROPERTY(EditInstanceOnly, meta = (AllowPrivateAccess = "true", ClampMin = "1", ClampMax = "5"))
	float ToastDuration = 3.f;

	UPROPERTY(EditInstanceOnly, meta = (AllowPrivateAccess = "true", ClampMin = "1", ClampMax = "10"))
	int ToastBoxMaximumCount = 10;

	UPROPERTY(EditInstanceOnly, meta = (AllowPrivateAccess = "true"))
	EToastStackType ToastStackType;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditInstanceOnly, meta = (AllowPrivateAccess = "true", ClampMin = "1", ClampMax = "10"))
	int PreviewEntryCount = 0;
#endif

protected:
	virtual void SynchronizeProperties() override;
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& InGeometry, float InDeltaTime) override;

public:
	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	void RegistToast(FGameplayTag InTag, const FText& InText);

private:
	void AddToast(const FGameplayTag& InTag, const FText& InText);
	void RemoveToast();

private:
	void OnEndedFadeOutAnimation(UToastMessageTextUI* InMessageText);


private:
	UPROPERTY(meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
	UDynamicEntryBox* ToastBox;

	TArray<TPair<FGameplayTag, double>> MessageQueue;
	TQueue<TPair<FGameplayTag, FText>> WaitingQueue;
};
