// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EcpLayer.h"
#include "EcpModalLayer.generated.h"


class UBorder;
class UNamedSlot;

UCLASS(MinimalAPI)
class UEcpModalLayer : public UEcpLayer
{
	GENERATED_BODY()
	
protected:
	virtual void OnClick() override;
	virtual void OnConstruct() override;

private:
	void SetBlackScreen(bool bIsVisible);

private:
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess, BindWidgetOptional))
	UBorder* BackGroundImage;


	FLinearColor DefaultColor;
};
