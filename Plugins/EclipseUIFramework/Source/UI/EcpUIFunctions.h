// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EcpUIFunctions.generated.h"


class APlayerController;
class ULocalPlayer;

UCLASS(MinimalAPI)
class UEcpUIFunctions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UEcpUIFunctions() {}

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Eclipse UI Helper Function")
	static void ShowLayerWidget(const ULocalPlayer* InLocalPlayer, EEclipseGameLayer InLayerType, TSoftClassPtr<UCommonActivatableWidget> InWidgetClass);

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Eclipse UI Helper Function")
	static void ShowLayerWidgetFromPlayerController(const APlayerController* InController, EEclipseGameLayer InLayerType, TSoftClassPtr<UCommonActivatableWidget> InWidgetClass);

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Eclipse UI Helper Function")
	static void HideLayerWidget(const ULocalPlayer* InLocalPlayer, UCommonActivatableWidget* InLayerWidget);

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Eclipse UI Helper Function")
	static void HideLayerWidgetFromPlayerController(const APlayerController* InController, UCommonActivatableWidget* InLayerWidget);

	
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Eclipse UI Helper Function")
	static void SetInputLockFromPlayerController(const APlayerController* InController, bool bLock);

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Eclipse UI Helper Function")
	static void SetInputLock(const ULocalPlayer* InLocalPlayer, bool bLock);
	
};
