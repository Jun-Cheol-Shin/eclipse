// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NLGameInstance.h"
#include "EpGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class ECLIPSE_API UEpGameInstance : public UNLGameInstance
{
	GENERATED_BODY()
	
	
private:
	virtual void Init() override;
	virtual void Shutdown() override;
	
};
