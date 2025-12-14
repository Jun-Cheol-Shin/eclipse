// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EclipseFunctionLibrary.generated.h"


class UEpGameDataSubsystem;
class UEpInputManagerSubsystem;
class UEpResourceSubsystem;
class UEpUIManagerSubsystem;

UCLASS()
class ECLIPSE_API UEclipseFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
		
public:
	UFUNCTION(BlueprintCallable, Category = "Eclipse Helper")
	static UEpGameDataSubsystem* GetGameDataSubSytem(UWorld* InWorld);

	UFUNCTION(BlueprintCallable, Category = "Eclipse Helper")
	static UEpInputManagerSubsystem* GetInputManagerSubSytem(ULocalPlayer* InLocalPlayer);

	UFUNCTION(BlueprintCallable, Category = "Eclipse Helper")
	static UEpResourceSubsystem* GetResourceSubSytem(UWorld* InWorld);

	UFUNCTION(BlueprintCallable, Category = "Eclipse Helper")
	static UEpUIManagerSubsystem* GetUIManagerSubSystem(UWorld* InWorld);

	
};
