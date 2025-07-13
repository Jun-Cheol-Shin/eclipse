// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "EcpGameLayout.generated.h"


struct FStreamableHandle;
class UCommonActivatableWidgetContainerBase;
class UCommonActivatableWidget;
class UEcpLayer;


UENUM(BlueprintType)
enum class EEclipseGameLayer : uint8
{
	System = 0,			// System Error Messsage												( Input Mode Is UIOnly )
	Production,			// Reward, Level Up, Success enchant, etc...							( Input Mode Is GameAndUI )
	Modal,				// Modal Popup Layer													( Input Mode Is UIOnly )
	Left_Sheet,			// NPC Interaction UI													( Input Mode Is GameAndUI )
	Right_Sheet,		// Diablo Inventory Layer												( Input Mode Is GameAndUI )
	Bottom_Sheet,		//																		( Input Mode Is GameAndUI )
	Window,				// Contents Window Widget												( Input Mode Is UIOnly )
	MainHUD,			// MainHUD Layer														( Input Mode Is GameOnly )

	Count,
};

UENUM()
enum class EAsyncWidgetState : uint8
{
	Canceled,
	Initialize,
	AfterPush
};


UCLASS(meta = (DisableNativeTick), MinimalAPI)
class UEcpGameLayout : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	TSharedPtr<FStreamableHandle> PushWidgetToLayerStackAsync(EEclipseGameLayer LayerName, bool bSuspendInputUntilComplete, TSoftClassPtr<UCommonActivatableWidget> ActivatableWidgetClass, TFunction<void(EAsyncWidgetState, UCommonActivatableWidget*)> StateFunc);
	UCommonActivatableWidget* PushWidgetToLayerStack(EEclipseGameLayer LayerName, UClass* ActivatableWidgetClass, TFunctionRef<void(UCommonActivatableWidget&)> InitInstanceFunc);

	void RemoveWidgetToLayerStack(UCommonActivatableWidget* InWidget);


private:
	// EcpLayer CallBack
	void OnChangedDisplayedWidget(UCommonActivatableWidget* InWidget, UEcpLayer* InLayer, bool bIsActivated);
	void RefreshGameLayerInputMode();

private:
	UCommonActivatableWidgetContainerBase* GetLayout(EEclipseGameLayer InLayer);

protected:
	UFUNCTION(BlueprintCallable)
	void RegistGameLayer(EEclipseGameLayer InLayerType, UEcpLayer* InLayer);
	
private:
	UPROPERTY(Transient)
	TMap<EEclipseGameLayer, UEcpLayer*> Layers;
	bool InputModeChecker[(uint8)EEclipseGameLayer::Count];
	
};
