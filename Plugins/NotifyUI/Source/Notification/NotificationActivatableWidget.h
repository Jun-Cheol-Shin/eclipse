// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "NotificationActivatableWidget.generated.h"

class UTextBlock;
class UImage;

USTRUCT(BlueprintType)
struct FNofiticationTextVariable
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	UImage* TopMessageIcon = nullptr;

	UPROPERTY(BlueprintReadWrite)
	UImage* MiddleMessageIcon = nullptr;

	UPROPERTY(BlueprintReadWrite)
	UImage* BottomMessageIcon = nullptr;

	UPROPERTY(BlueprintReadWrite)
	UTextBlock* TopMessageText_1 = nullptr;

	UPROPERTY(BlueprintReadWrite)
	UTextBlock* TopMessageText_2 = nullptr;

	UPROPERTY(BlueprintReadWrite)
	UTextBlock* TopMessageText_3 = nullptr;

	UPROPERTY(BlueprintReadWrite)
	UTextBlock* MiddleMessageText_1 = nullptr;

	UPROPERTY(BlueprintReadWrite)
	UTextBlock* MiddleMessageText_2 = nullptr;

	UPROPERTY(BlueprintReadWrite)
	UTextBlock* MiddleMessageText_3 = nullptr;

	UPROPERTY(BlueprintReadWrite)
	UTextBlock* BottomMessageText_1 = nullptr; 

	UPROPERTY(BlueprintReadWrite)
	UTextBlock* BottomMessageText_2 = nullptr;

	UPROPERTY(BlueprintReadWrite)
	UTextBlock* BottomMessageText_3 = nullptr;	
};


UCLASS(MinimalAPI)
class UNotificationActivatableWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()


protected:





	
	
private:
	UPROPERTY(meta = (AllowPrivateAccess = "true", BindWidget))
	UWidget* TopMessageBox;

	UPROPERTY(meta = (AllowPrivateAccess = "true", BindWidget))
	UWidget* MiddleMessageBox;

	UPROPERTY(meta = (AllowPrivateAccess = "true", BindWidget))
	UWidget* BottomMessageBox;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FNofiticationTextVariable UIVariable;
};
