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


public:
	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	void RegistToastInt(FGameplayTag InTag, const FString& InFormat, const TArray<int64>& InArguments);

	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	void RegistToastDouble(FGameplayTag InTag, const FString& InFormat, const TArray<double>& InArguments);


	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	void RegistToastFloat(FGameplayTag InTag, const FString& InFormat, const TArray<float>& InArguments);

	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	void RegistToastUInt(FGameplayTag InTag, const FString& InFormat, const TArray<uint8>& InArguments);

	// not merge
	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	void RegistToastText(FGameplayTag InTag, const FText& InText);

private:
	void RegistToastInternal(FGameplayTag InTag, const FTextFormat& InFormat, FFormatOrderedArguments InArguments);
	void RegistToastInternal(FGameplayTag InTag, const FText& InText);

private:
	UPROPERTY(EditInstanceOnly, meta = (AllowPrivateAccess = "true", ClampMin = "1", ClampMax = "5"))
	float ToastDuration = 3.f;

	UPROPERTY(EditInstanceOnly, meta = (AllowPrivateAccess = "true", ClampMin = "1", ClampMax = "10"))
	int ToastBoxMaximumCount = 10;

	UPROPERTY(EditInstanceOnly, meta = (AllowPrivateAccess = "true"))
	EToastStackType ToastStackType;

	UPROPERTY(EditInstanceOnly, meta = (AllowPrivateAccess = "true", ClampMin = "0.2", ClampMax = "0.5"))
	bool bMerge = true;

	UPROPERTY(EditInstanceOnly, meta = (AllowPrivateAccess = "true", ClampMin = "0.2", ClampMax = "0.5", EditCondition = "true == bMerge", EditConditionHides))
	float ToastMergeInterval = 0.3f;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditInstanceOnly, meta = (AllowPrivateAccess = "true", ClampMin = "1", ClampMax = "10"))
	int PreviewEntryCount = 0;
#endif

protected:
	virtual void SynchronizeProperties() override;
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& InGeometry, float InDeltaTime) override;


private:
	void AddToast();
	//void RemoveToast(const FGameplayTag& InTag);
	void RemoveToast(int Index);

private:
	void OnEndedFadeOutAnimation(UToastMessageTextUI* InMessageText, const FGameplayTag& InMessageType);


private:
	UPROPERTY(meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
	UDynamicEntryBox* ToastBox;

	TArray<TPair<double /* Regist Time */, bool /* Is Expired? */>> TimeQueue;
	TQueue<FToastMessageData> WaitQueue;

	TPair<FToastMessageData, double> NeedMergeData = { FToastMessageData(), 0.f};
};
