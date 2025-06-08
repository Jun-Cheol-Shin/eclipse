// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "EcpGameLayout.generated.h"


class UCommonActivatableWidgetContainerBase;
class UCommonActivatableWidget;

UENUM(BlueprintType)
enum class EEclipseGameLayer : uint8
{
	System = 0,
	Modal,
	Left_Sheet,
	Right_Sheet,
	Window,
};

UCLASS(meta = (DisableNativeTick))
class ECLIPSE_API UEcpGameLayout : public UCommonUserWidget
{
	GENERATED_BODY()
	

public:
	DECLARE_DELEGATE_TwoParams(FOnCompleteDisplayedWidget, EEclipseGameLayer, UCommonActivatableWidget*);
		FOnCompleteDisplayedWidget OnCompleteDisplayedWidget;

	static UEcpGameLayout* GetGameLayout(APlayerController* InPlayerController);
	static UEcpGameLayout* GetGameLayout(ULocalPlayer* InPlayer);


protected:
	// UCommonActivatableWidgetContainerBase Event
	void OnDisplayedWidgetChanged(UCommonActivatableWidget* InWidget);
	void OnChangedTransitioning(UCommonActivatableWidgetContainerBase* InLayer, bool bIsTransitioning);


protected:
	UFUNCTION(BlueprintCallable)
	void RegistGameLayer(EEclipseGameLayer InLayer, UCommonActivatableWidgetContainerBase* InContainer);
	
private:
	UPROPERTY(Transient)
	TMap<EEclipseGameLayer, TObjectPtr<UCommonActivatableWidgetContainerBase>> Layers;
	
};
