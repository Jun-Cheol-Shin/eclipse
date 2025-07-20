// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "ToastMessageBoxUI.generated.h"


class UDynamicEntryBox;

UENUM(BlueprintType)
enum class EToastDirType : uint8
{
	Up,
	Down,
};

UCLASS()
class NOTIFYUI_API UToastMessageBoxUI : public UCommonUserWidget
{
	GENERATED_BODY()
	
	
private:
	UPROPERTY(EditInstanceOnly, meta = (AllowPrivateAccess = "true", ClampMin = "1", ClampMax = "10"))
	int ToastBoxMaximumCount = 10;
	
private:
	UPROPERTY(meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
	UDynamicEntryBox* ToastBox;
};
